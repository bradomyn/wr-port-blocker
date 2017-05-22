#include "Conf.h"

Conf::Conf(std::string Conf_file)
{
        if (!Conf_file.empty())
                Conf_p = Conf_file;
}

int Conf::CloseConf()
{
        Conf_f.close();
        return 0;
}

int Conf::GetConf()
{
        Conf_f.open(Conf_p); //open in constructor

        if (Conf_f) {
                std::string str = "";
                while (std::getline(Conf_f, str, '='))
                {
                        if (config_map.find(std::string(str)) != config_map.end())
                        {
                                std::string value="";
                                if( std::getline(Conf_f, value) )
                                {
                                        config_map[str] = value;
                                }
                        }
                }

        }
        else {
                LogError("Config File " + Conf_p + " not found");
                return (-1);
        }
        CloseConf();

        return 0;
}


