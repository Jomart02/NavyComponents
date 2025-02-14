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