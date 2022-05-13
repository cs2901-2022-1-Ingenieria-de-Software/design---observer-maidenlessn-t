#include <iostream>
#include <list>
#include <ctime>
#include <vector>
#include <string>
using namespace std;


class DisplayDevice {
    private:
        double displayed_temperature;
        double displayed_pressure;
        double displayed_humidity;
        string displayed_statistic;
        string displayed_forecast;

    public:
        DisplayDevice(){
            displayed_temperature = 0;
            displayed_pressure = 0;
            displayed_humidity = 0;
            displayed_statistic = "";
            displayed_forecast = "";
        }
        void setPressure(double p){
            displayed_pressure = p;
        }
        void setTemperature(double t){
            displayed_temperature = t;
        }
        void setHumidity(double h){
            displayed_humidity = h;
        } 
        void setStatistic(string s){
            displayed_statistic = s;
        } 
        void setForecast(string f){
            displayed_forecast = f;
        } 

        void displaydata() {
            printf("Temperature: %f, Pressure: %f, Humidity: %f\n", displayed_temperature, displayed_pressure, displayed_humidity);
            cout << "Forecast: " << displayed_forecast << "\n";
            cout << "Statistic: " << displayed_statistic << "\n";
        }
};

class Observer{
    public:
        virtual bool update() = 0;
};

class WeatherStation{
    private:
        double temperature;
        double pressure;
        double humidity;
    public:
        list<Observer*> observerList;
        
        void registerObserver(Observer* _o){
            observerList.push_back(_o);
        }
        void removeObserver(){
            observerList.pop_back();
        }
        void notifyObserver(){
            for (auto & i : observerList) {
                i->update();
            } 
        }

        double getTemperatureState(){
            return temperature;
        };
        double getPressureState(){
            return pressure;
        };
        double getHumidityState(){
            return humidity;
        };

        void setState(double t, double p, double h){
            this->temperature = t;
            this->pressure = p;
            this->humidity = h;
            this->notifyObserver();
        };
        
        void genState(){
            srand(time(0));
            double t = ((rand() % 20) + 20) + ((rand() % 100) / 100);
            double p = ((rand() % 2) + 1) + ((rand() % 100) / 100);
            double h = (rand() % 100) + ((rand() % 100) / 100);
            setState(t, p, h);
        };
};

class ForecastObserver : public Observer{
    private: 
        DisplayDevice *setDisplay;
        WeatherStation *weatherStation;
        
        vector<double> humidity_log; 
        vector<double> temperature_log; 
        vector<double> pressure_log; 
        
        double average_humidity;
        double average_temperature;
        double average_pressure;

        void set_average() {
            humidity_log.push_back(weatherStation->getHumidityState());
            pressure_log.push_back(weatherStation->getPressureState());
            temperature_log.push_back(weatherStation->getTemperatureState());

            if(humidity_log.size() > 20){
                humidity_log.erase(humidity_log.begin());
                temperature_log.erase(temperature_log.begin());
                pressure_log.erase(pressure_log.begin());
            }

            double temp;
            for (auto i : humidity_log)
                temp = temp + i;
            average_humidity = temp/humidity_log.size();

            for (auto i : temperature_log)
                temp = temp + i;
            average_temperature = temp/temperature_log.size();
            
            for (auto i : pressure_log)
                temp = temp + i;
            average_pressure = temp/pressure_log.size();
        }
        
        string summary() {
            string result = " -The forecast for today is, ";
            if (average_humidity > 50){
                result += "WET and ";
            }else{
                result += "DRY and ";
            }

            if (average_temperature > 30){
              result += "HOT and ";
            }else{
              result += "SUNY and ";                
            }

            if (average_pressure > 1.5){
                result += "SUNNY";   
            }else {
                result += "CLOUDY";
            }
            return result;
        }
        
        public:
        ForecastObserver(WeatherStation* _w, DisplayDevice *_d){
            setDisplay = _d;
            weatherStation = _w;
        };

        bool update()  override{
            set_average();
            setDisplay->setForecast(summary());

            return 1;
        }
};

class ConditionObserver : public Observer{
    private: 
        DisplayDevice *setDisplay;
        WeatherStation *weatherStation;
    public:
        ConditionObserver(WeatherStation* _w, DisplayDevice *_d){
            setDisplay = _d;
            weatherStation = _w;
        };
        
        bool update()  override{
            setDisplay->setHumidity(weatherStation->getHumidityState());
            setDisplay->setPressure(weatherStation->getPressureState());
            setDisplay->setTemperature(weatherStation->getTemperatureState());
            return 1;
        }
};

class StatisticsObserver : public Observer{
    private: 
        vector<double> humidity;
        vector<double> pressure;
        vector<double> temperature;
        DisplayDevice *setDisplay;        
        WeatherStation *weatherStation;
        
        double avg_hum;
        double avg_temp;
        double avg_pres;
        
    public:
        StatisticsObserver(WeatherStation* _w, DisplayDevice *_d){
            setDisplay = _d;
            weatherStation = _w;
        };
                
        bool update() override{       
            getData();
            avg_hum = getAverage(humidity);
            avg_temp = getAverage(temperature);
            avg_pres = getAverage(pressure);
            
            setDisplay->setStatistic(summary_stats());
            return 1;
        }

        void getData() {
            humidity.push_back(weatherStation->getHumidityState());
            pressure.push_back(weatherStation->getPressureState());
            temperature.push_back(weatherStation->getTemperatureState());    
            
            if(humidity.size() > 20){
                humidity.erase(humidity.begin());
                temperature.erase(temperature.begin());
                pressure.erase(pressure.begin());
            }  
        }

     
        double getAverage (vector<double> value) {
            int len = value.size();
            double total;
            for (auto i : value){
                total = total + i;
            }
            return total/len;
        }

        string summary_stats() {
            string result = "-The average humidity in the last 20 days is ";
            result += to_string(avg_hum);

            result += "\n\t   -The average temperature in the last 20 days is ";
            result += to_string(avg_temp);

            result += "\n\t   -The average pressure in the last 20 days is ";
            result += to_string(avg_pres);

            return result;
        } 
};

int main(){
    DisplayDevice Display;
    WeatherStation Station;
    Station.registerObserver(new ConditionObserver(&Station, &Display));
    Station.registerObserver(new ForecastObserver(&Station, &Display));
    Station.registerObserver(new StatisticsObserver(&Station, &Display));
    
    Display.displaydata();
    Station.genState();
    Display.displaydata();
    Station.genState();
    Display.displaydata();

    

    for (auto i = 0; i<100; i++)
        Station.genState();
    Display.displaydata();
};  