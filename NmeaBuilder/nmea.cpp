#include "nmea.h"

Nmea::Nmea(std::string header){
    this->header = header;
    nmea_data.push_back(header);
}

Nmea::Nmea(std::string header, std::vector<std::string> data){
    this->header = header;
    nmea_data.push_back(header);
    this->nmea_data.insert(nmea_data.end(),data.begin(),data.end());
}

Nmea::Nmea(std::string header, std::string data[], unsigned int SIZE){
    this->header = header;
    nmea_data.push_back(header);
    for(int i=0;i<SIZE;i++) nmea_data.push_back(data[i]);
}

void Nmea::push(std::string new_value){
    nmea_data.push_back(new_value);
}

void Nmea::set(unsigned int pos, std::string value){
    nmea_data[pos] = value;
}

void Nmea::get_checksum(std::string data, char *result){
    unsigned int checksum = 0;
    for(char c: data){
        checksum = checksum ^ (int) c;
    }
    sprintf(result,"%02X",checksum);
}

std::string Nmea::get_string(){
    std::string result = "";
    for(int i=0;i<nmea_data.size();i++){
        result+=nmea_data[i];
        if(i!=nmea_data.size()-1) result+=",";
    }
    char checksum[3]{};
    get_checksum(result,checksum);
    result+='*';
    result+=checksum;
    result+="\r\n";
    result.insert(result.begin(),'$');
    return result;
}


void helpFuncNmea::formatLatitude(double lat, std::string& latString, std::string& direction) {
    int degrees = static_cast<int>(std::abs(lat)); // Градусы
    double minutes = (std::abs(lat - degrees)) * 60.0; // Минуты

    if (degrees > 90) {
        latString = "0000.00";
        direction = "N";
        return;
    }

    // Разделяем минуты на целую и дробную часть
    int minInt = static_cast<int>(minutes);
    double minFrac = minutes - minInt;

    // Округление до двух знаков после запятой
    int fracTenths = static_cast<int>(minFrac * 100 + 0.5); // +0.5 для округления
    if (fracTenths >= 100) {
        minInt += 1;
        fracTenths = 0;
    }

    char buffer[16];
    snprintf(buffer, sizeof(buffer), "%02d%02d.%02d", degrees, minInt, fracTenths);

    latString = buffer;
    direction = (lat >= 0) ? "N" : "S";
}

// Форматирует долготу в строку формата dddmm.mm и возвращает направление (E/W)
void helpFuncNmea::formatLongitude(double lon, std::string& lonString, std::string& direction) {
    int degrees = static_cast<int>(std::abs(lon));
    double minutes = (std::abs(lon - degrees)) * 60.0;

    if (degrees > 180) {
        lonString = "00000.00";
        direction = "E";
        return;
    }

    int minInt = static_cast<int>(minutes);
    double minFrac = minutes - minInt;
    int fracTenths = static_cast<int>(minFrac * 100 + 0.5);

    if (fracTenths >= 100) {
        minInt += 1;
        fracTenths = 0;
    }

    char buffer[16];
    snprintf(buffer, sizeof(buffer), "%03d%02d.%02d", degrees, minInt, fracTenths);

    lonString = buffer;
    direction = (lon >= 0) ? "E" : "W";
}