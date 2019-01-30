#include "common.h"
#include "GlobalObjects.h"

SDL_Window *mainWindow = NULL;
SDL_GLContext mainContext = NULL,cairoContext = NULL;
bool quitGame = false;
bool vsyncEnabled = false;

GLuint vertexShader,fragmentShader;
GLuint shaderProgram;
GLuint emptyTexture;

Renderer renderer;

FT_Library ftLibrary;

std::mt19937 randEngine(std::time(0));

bool GlobalInitialize(){
  //SDL Initialization
  if(SDL_Init(SDL_INIT_VIDEO) < 0){
    SDL_Log("SDL Initialization failed, %s\n",SDL_GetError());
    return false;
  }

  if(IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG){
    SDL_Log("SDL Image Initialization failed, %s\n",IMG_GetError());
    return false;
  }

  if(SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER,1) != 0){
    SDL_Log("%s\n",SDL_GetError());
    return false;
  }
  if(SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE,8) != 0){
    SDL_Log("%s\n",SDL_GetError());
    return false;
  }
  if(SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION,3) != 0){
    SDL_Log("%s\n",SDL_GetError());
    return false;
  }
  if(SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION,2) != 0){
    SDL_Log("%s\n",SDL_GetError());
    return false;
  }
  if(SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,SDL_GL_CONTEXT_PROFILE_CORE) != 0){
    SDL_Log("%s\n",SDL_GetError());
    return false;
  }

  mainWindow = SDL_CreateWindow(
				"The Demo Game, by Charlie and Gilbert",
				SDL_WINDOWPOS_CENTERED,
				SDL_WINDOWPOS_CENTERED,
				800,600,
				SDL_WINDOW_OPENGL|SDL_WINDOW_FULLSCREEN_DESKTOP
			       );
  if(mainWindow == NULL){
    SDL_Log("Create Window failed, %s\n",SDL_GetError());
    return false;
  }

  mainContext = SDL_GL_CreateContext(mainWindow);
  if(mainContext == NULL){
    SDL_Log("Create OpenGL Context failed, %s\n",SDL_GetError());
    return false;
  }

  SDL_GL_SetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT,1);

  glewExperimental = GL_TRUE;
  GLenum err = glewInit();
  if(err != GLEW_OK){
    std::cout << "GLEW Initialization failed, " << glewGetErrorString(err) << std::endl;
    return false;
  }

  if(SDL_GL_SetSwapInterval(1) == 0){
    vsyncEnabled = true;
    std::cout << "VSync Enabled" << std::endl;
  } else {
    std::cout << "VSync Not Enabled" << std::endl;
  }

  FT_Init_FreeType(&ftLibrary);

  renderer.Initialize();

  //Create Shader Program
  int fileLen;
  char *shaderlog;

  vertexShader = glCreateShader(GL_VERTEX_SHADER);
  char* vertexSource = ReadWholeFile("PZQGame.vert",&fileLen);
  glShaderSource(vertexShader,1,(const GLchar**)&vertexSource,&fileLen);
  glCompileShader(vertexShader);
  glGetShaderiv(vertexShader,GL_INFO_LOG_LENGTH,&fileLen);
  shaderlog = new char[fileLen + 50];
  glGetShaderInfoLog(vertexShader,fileLen,&fileLen,shaderlog);
  std::cout << shaderlog << std::endl;
  
  delete [] vertexSource;
  delete [] shaderlog;

  fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  char* fragmentSource = ReadWholeFile("PZQGame.frag",&fileLen);
  glShaderSource(fragmentShader,1,(const GLchar**)&fragmentSource,&fileLen);
  glCompileShader(fragmentShader);
  glGetShaderiv(fragmentShader,GL_INFO_LOG_LENGTH,&fileLen);
  shaderlog = new char[fileLen + 50];
  glGetShaderInfoLog(fragmentShader,fileLen,&fileLen,shaderlog);
  std::cout << shaderlog << std::endl;
  
  delete [] fragmentSource;
  delete [] shaderlog;

  shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram,vertexShader);
  glAttachShader(shaderProgram,fragmentShader);
  glLinkProgram(shaderProgram);
  glGetProgramiv(shaderProgram,GL_INFO_LOG_LENGTH,&fileLen);
  shaderlog = new char[fileLen + 50];
  glGetProgramInfoLog(shaderProgram,fileLen,&fileLen,shaderlog);
  std::cout << shaderlog << std::endl;
  
  delete [] shaderlog;
  
  glUseProgram(shaderProgram);
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);

  glClearColor(0.0,0.0,0.0,1.0);
  glClearStencil(0);
  glStencilFunc(GL_LEQUAL,1,0xFF);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

  glGenTextures(1,&emptyTexture);
  glBindTexture(GL_TEXTURE_2D,emptyTexture);
  glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,1,1,0,GL_BGRA,GL_UNSIGNED_BYTE,NULL);
  glBindTexture(GL_TEXTURE_2D,0);
  return true;
}

void GlobalFinalize(){
  FT_Done_FreeType(ftLibrary);
  glUseProgram(0);
  glDeleteProgram(shaderProgram);
  SDL_GL_DeleteContext(mainContext);
  SDL_DestroyWindow(mainWindow);
  SDL_Quit();
}

char* ReadWholeFile(const char *filename,int *length){
  std::FILE *f = fopen(filename,"rb");
  if(f){
    char *content;
    std::fseek(f,0,SEEK_END);
    int len = std::ftell(f);
    if(length) *length = len;
    content = new char[len + 1];
    std::rewind(f);
    std::fread(content,1,len,f);
    content[len] = 0;
    std::fclose(f);
    return content;
  } else {
    return NULL;
  }
}