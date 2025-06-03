#ifndef NMEA_H
#define NMEA_H

#include <string>
#include <vector>

class Nmea{
    private:
        std::string header;
        std::vector<std::string> nmea_data;
    public:
        Nmea(std::string header);
        Nmea(std::string header, std::vector<std::string> data);
        Nmea(std::string header, std::string data[], unsigned int size);
        void push(std::string new_value);
        void set(unsigned int pos, std::string value);
        void get_checksum(std::string data, char *result);
        std::string get_string();
};


namespace helpFuncNmea {
    // Форматирует широту в строку формата ddmm.mm и возвращает направление (N/S)
    void formatLatitude(double lat, std::string& latString, std::string& direction) ;
    // Форматирует долготу в строку формата dddmm.mm и возвращает направление (E/W)
    void formatLongitude(double lon, std::string& lonString, std::string& direction);

}

#endif 