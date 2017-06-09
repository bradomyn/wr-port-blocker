#include "Conf.h"

Conf::Conf(std::string ConfFile, int LogLevel)
        : Log(LogLevel)
{
        if (!ConfFile.empty())
                ConfP = ConfFile;
}

int Conf::CloseConf()
{
        ConfF.close();
        return 0;
}

int Conf::GetConf()
{
        ConfF.open(ConfP); //open in constructor
        size_t ParamIdx;
        std::string Param;
        std::string Value;

        if (ConfF) {
                std::string str = "";
                while (std::getline(ConfF, str))
                {
                        if (str.at(0) != '#') { // it is not a comment line

                                ParamIdx = str.find('=');
                                Param = str.substr(0,ParamIdx);
                                if (ConfigMap.find(std::string(Param)) != ConfigMap.end())
                                {
                                        Value=str.substr(ParamIdx+1,str.size());
                                        ConfigMap[Param] = Value;
                                        LogDebug("Config Parameter:\t" + Param + "=" + Value);
                                }
                        }
                }
                LogInfo("Config File " + ConfP + " loaded correctly");
        }
        else {
                LogError("Config File " + ConfP + " not found");
                return (-1);
        }
        CloseConf();

        return 0;
}


