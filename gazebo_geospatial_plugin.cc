#include <ignition/plugin/Register.hh>
#include <ignition/rendering/Visual.hh>
#include <ignition/rendering/Scene.hh>

#include <curl/curl.h>
#include <jsoncpp/json/json.h>

namespace gazebo
{
  class OSMPlugin : public WorldPlugin
  {
  public:
    void Load(physics::WorldPtr _world, sdf::ElementPtr _sdf) override
    {
      // Get the latitude and longitude from the SDF parameters
      double lat = _sdf->Get<double>("latitude");
      double lon = _sdf->Get<double>("longitude");

      // Create a URL to fetch the map data for the specified location
      std::string url = "https://nominatim.openstreetmap.org/reverse?format=json&lat="
                        + std::to_string(lat) + "&lon=" + std::to_string(lon);

      // Fetch the JSON data from the URL
      std::string json;
      CURL* curl = curl_easy_init();
      if (curl)
      {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &json);
        curl_easy_perform(curl);
        curl_easy_cleanup(curl);
      }

      // Parse the JSON data to get the map tile URL
      std::string tileUrl;
      Json::Value root;
      Json::CharReaderBuilder builder;
      std::istringstream iss(json);
      JSONCPP_STRING err;
      if (Json::parseFromStream(builder, iss, &root, &err))
      {
        tileUrl = "https://tile.openstreetmap.org/" + root["address"]["postcode"].asString() +
                  "/" + root["address"]["postcode"].asString() + ".png";
      }

      // Add the map tile as a visual to the Gazebo world
      rendering::ScenePtr scene = rendering::get_scene();
      rendering::VisualPtr visual = scene->CreateVisual();
      visual->SetLocalScale(math::Vector3(100, 100, 1));
      visual->Load();
      rendering::MaterialPtr material = visual->CreateMaterial("material");
      material->SetShader("Gazebo/Diffuse");
      material->SetAmbient(common::Color::White);
      material->SetDiffuse(common::Color::White);
      material->SetTexture(tileUrl);
      visual->SetMaterial(material);
      visual->SetWorldPose(math::Pose(math::Vector3(0, 0, 0), math::Quaternion::Identity));
      scene->AddVisual(visual);
    }

  private:
    static size_t writeCallback(void* contents, size_t size, size_t nmemb, void* userp)
    {
      size_t realsize = size * nmemb;
      std::string* str = static_cast<std::string*>(userp);
      str->append(static_cast<char*>(contents), realsize);
      return realsize;
    }
  };

  GZ_REGISTER_WORLD_PLUGIN(OSMPlugin)
}