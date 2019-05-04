#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <bass/bass.h>
#include <rocket/sync.h>

#include <utils/shader.h>

#include <cmath>
#include <fstream>
#include <iostream>
#include <string>
#include <thread>
#include <map>

#define RES_X 1252
#define RES_Y 704

/// Holds all state information relevant to a character as loaded using FreeType
struct Character {
    GLuint TextureID;   // ID handle of the glyph texture
    glm::ivec2 Size;    // Size of glyph
    glm::ivec2 Bearing;  // Offset from baseline to left/top of glyph
    GLuint Advance;    // Horizontal offset to advance to next glyph
};
std::map<GLchar, Character> Characters;


/**** ROCKET RELATED VARS & FUNCTIONS ****/
static const float bpm = 140.0f; /* beats per minute */
static const int rpb = 8; /* rows per beat */
static const double row_rate = (double(bpm) / 60) * rpb;
static double bass_get_row(HSTREAM h) {
	QWORD pos = BASS_ChannelGetPosition(h, BASS_POS_BYTE);
	double time = BASS_ChannelBytes2Seconds(h, pos);
	return time * row_rate;
}
static void bass_pause(void *d, int flag) {
	HSTREAM h = *((HSTREAM *)d);
	if (flag)
		BASS_ChannelPause(h);
	else
		BASS_ChannelPlay(h, false);
}
static void bass_set_row(void *d, int row) {
	HSTREAM h = *((HSTREAM *)d);
	QWORD pos = BASS_ChannelSeconds2Bytes(h, row / row_rate);
	BASS_ChannelSetPosition(h, pos, BASS_POS_BYTE);
}
static int bass_is_playing(void *d) {
	HSTREAM h = *((HSTREAM *)d);
	return BASS_ChannelIsActive(h) == BASS_ACTIVE_PLAYING;
}
static struct sync_cb bass_cb = {
	bass_pause,
	bass_set_row,
	bass_is_playing
};
/**** ROCKET END ****/

void processInput(GLFWwindow *window);
void processInput(GLFWwindow *window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  glViewport(0, 0, width, height);
}


void titlecard(unsigned int VAO, Shader shader, float row);
void titlecard(unsigned int VAO, Shader shader, float row) {
  // Clear screen: Blue
  glClearColor(0.01f, 0.0f, 0.99f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  shader.setVec2("iResolution", RES_X, RES_Y);
  shader.setFloat("iTime", (float)row);
  

  shader.use();

  glBindVertexArray(VAO);
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}


void marchBall(unsigned int VAO, Shader shader, const float &row, const float &focalScale, const glm::vec4 &ballPos, const glm::vec3 &lightPos, const float& staticYes);
void marchBall(unsigned int VAO, Shader shader, const float &row, const float &focalScale, const glm::vec4 &ballPos, const glm::vec3 &lightPos, const float& staticYes) {
  // Clear screen
  glClearColor(0.00f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  shader.setVec2("iResolution", RES_X, RES_Y);
  shader.setFloat("iTime", row);
  shader.setFloat("focal_scale", focalScale);
  shader.setVec4("ball_pos", ballPos);
  shader.setVec3("light_pos", lightPos);
  shader.setFloat("static_yes", staticYes);

  // Draw shape
  shader.use();

  glBindVertexArray(VAO);
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}


int main() {
  HSTREAM stream;
  int error_code;

  /* init BASS */
  if (!BASS_Init(-1, 44100, 0, 0, 0)) {
    std::cerr << "Failed to init bass" << std::endl;;
    return -1;
  }
  stream = BASS_StreamCreateFile(false, "../circlontia.ogg", 0, 0,
				 BASS_STREAM_PRESCAN);
  if (!stream) {
    error_code = BASS_ErrorGetCode();
    std::cerr << "Failed to open tune: " << error_code << std::endl;;
    return -1;
  }
  
  /* init rocket */
  sync_device *rocket = sync_create_device("sync");
  if (!rocket) {
    std::cerr << "Out of memory?" << std::endl;
    return -1;
  }

  if (sync_tcp_connect(rocket, "localhost", SYNC_DEFAULT_PORT)) {
    std::cerr << "Failed to connect to host" << std::endl;
  }

  /* get rocket tracks */
  const sync_track *glob_state = sync_get_track(rocket, "global:state");
  const sync_track *focal_scale = sync_get_track(rocket, "global:focal_scale");
  const sync_track *static_yes = sync_get_track(rocket, "global:static_yes");
  const sync_track *dummy1 = sync_get_track(rocket, "global:dummy1");
  const sync_track *ball_x = sync_get_track(rocket, "ball:x");
  const sync_track *ball_y = sync_get_track(rocket, "ball:y");
  const sync_track *ball_z = sync_get_track(rocket, "ball:z");
  const sync_track *ball_r = sync_get_track(rocket, "ball:r");
  const sync_track *dummy2 = sync_get_track(rocket, "ball:dummy2");
  const sync_track *light_x = sync_get_track(rocket, "light:x");
  const sync_track *light_y = sync_get_track(rocket, "light:y");
  const sync_track *light_z = sync_get_track(rocket, "light:z");
  const sync_track *dummy3 = sync_get_track(rocket, "light:dummy3");

  /*
   * Window Context setup
   */
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow *window =
      glfwCreateWindow(RES_X, RES_Y, "Demo1", NULL, NULL);
  if (window == NULL) {
    std::cout << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cout << "Failed to initialize GLAD" << std::endl;
    return -1;
  }

  glViewport(0, 0, RES_X, RES_Y);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);


  /*
   * Sample scene setup
   */
  // Covers screen
  float vertices[] = {
      // Upper Tri
      -1.0f, 1.0f,  0.0f, // top left corner
      1.0f,  1.0f,  0.0f, // top right corner
      1.0f,  -1.0f, 0.0f, // bot right corner
      -1.0f, -1.0f, 0.0f  // bot left corner
  };
  unsigned int indicies[] = {0, 1, 2, 0, 2, 3};

  /// Buffers
  // Gen. Buffers
  unsigned int VBO, VAO, EBO;
  glGenBuffers(1, &VBO);
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &EBO);
  // Bind buffers, VAO first
  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicies), indicies,
	       GL_STATIC_DRAW);
  // Link Vert Attribs
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);
  // Unbind VAO to prevent future modification
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  /// Shaders
  // Vert Shader
  Shader ballShader("../basic.vert", "../rm_test.frag");
  Shader quineShader("../basic.vert", "../quine.frag");

  /* LET'S  ROLLLL */
  BASS_Start();
  BASS_ChannelPlay(stream, false);
  bool done = false;

  while (!glfwWindowShouldClose(window)) {
    processInput(window);

    /* Get rocket values */
    double row = bass_get_row(stream);
    /* Sync actions */
    if (sync_update(rocket, (int)floor(row), &bass_cb, (void *)&stream))
      sync_tcp_connect(rocket, "localhost", SYNC_DEFAULT_PORT);


    glClearColor(0.01f, 0.0f, 0.99f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    /* OPENGL  Choose effect:  -------------------------------------- */
    int state = int(sync_get_val(glob_state, row));
    if(state == 0)
      titlecard(VAO, quineShader, row);
    else if (state == 1)
      marchBall(
		VAO,
		ballShader,
		row,
		sync_get_val(focal_scale, row),
		glm::vec4(
			  sync_get_val(ball_x, row),
			  sync_get_val(ball_y, row),
			  sync_get_val(ball_z, row),
			  sync_get_val(ball_r, row)
			  ),
		glm::vec3(
			  sync_get_val(light_x, row),
			  sync_get_val(light_y, row),
			  sync_get_val(light_z, row)
			  ),
		sync_get_val(static_yes, row)
		);

    BASS_Update(0); // decreases the change of missing vsync

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  sync_save_tracks(rocket);
  sync_destroy_device(rocket);
  BASS_StreamFree(stream);
  BASS_Free();

  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteBuffers(1, &EBO);
  glfwTerminate();

  return 0;
}
