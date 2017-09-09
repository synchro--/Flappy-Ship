#include "elements.hxx"


//Implementation of the objects in elements.hxx
namespace elements {
   Floor::Floor(const char *texture_filename)
   : m_size(100.0f),
     m_height(0.0f),
     m_env(agl::get_env()),
     // repat = true, linear interpolation
     m_tex(m_env.loadTexture(texture_filename, true)) {}

    void render {
      lg::i(__func__, "Rendering floor...");
      m_env.drawFloor(m_tex,  m_size, m_height, 150);
    }

   Floor* get_floor(const char *texture_filename) {
      const static auto TAG = __func__;
      lg::i(TAG, "Loading floor texture from %s", texture_filename);

      static std::unique_ptr<Floor> s_floor(nullptr);
      if(!s_floor) {
       s_floor.reset(new Floor(texture_filename)); //Init
      }

      return s_floor.get();
    }

  Sky::Sky(const char *texture_filename)
  : m_radius(100.0f),
    m_lats(20.0f),
    m_longs(20.0f),
    m_env(agl::get_env()),
    m_tex(m_env.loadTexture(texture_filename, false)) {}

    void render {
      lg::i(__func__, "Rendering Sky...");
      m_env.drawSky(m_tex, m_radius, m_lats, m_longs);
    }

   void set_params(double radius, int lats, int longs)
                       {
                         m_radius = radius;
                         m_lats = lats;
                         m_longs = longs;
                       }
                
    Floor* get_sky(const char *texture_filename) {
      const static auto TAG = __func__;
      lg::i(TAG, "Loading Sky texture from %s", texture_filename);

      static std::unique_ptr<Sky> s_sky(nullptr);
      if(!s_sky) {
       s_sky.reset(new Sky(texture_filename)); //Init
      }

      return s_sky.get();
    }
 
    
    Spaceship::Spaceship(const char *texture_filename, const char *mesh_filename) //da finire 
    : m_tex(0), // no texture for now 
      m_env(agl::get_env),
      m_mesh(agl::loadMesh(mesh_filename)),//TODO
      m_cmds(new std::std::queue<Spaceship::Command>) 
      { } //empty cons
      
      
      
      Spaceship* get_spaceship(const char *texture_filename, const char *mesh_filename) {
              const static auto TAG = __func__;
      lg::i(TAG, "Loading Spaceship --> texture: %s Mesh: %s", texture_filename, mesh_filename);

      static std::unique_ptr<Sky> s_Spaceship(nullptr);
      if(!s_sky) {
        s_Spaceship.reset(new Spaceship(texture_filename, mesh_filename)); //Init
      }

      return s_Spaceship.get();
      }
      
      
      void render(){} 
      
      bool executeCommand() {
        const static auto TAG = __func__;
        //read and pop command 
        Command cmd = m_cmds.front(); 
        m_cmds.pop(); 
        
        //get command name in string in order to log
        std::string mt = Spaceship::motion_to_str(cmd.first); 
        lg::i(TAG, "Spaceship is executing command %s", mt)
        
        //set the state
        
        m_state[cmd.first] = cmd.second; 
      }
}
