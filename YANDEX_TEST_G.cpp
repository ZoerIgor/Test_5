#include <iostream>
#include <algorithm>
#include <vector>
#include <stack>
#include <iterator>

#define FORMAT_ENTER

using namespace std;

enum EnumDirection { NORTH = 1, NORTHEAST, EAST, SOUTHEAST, SOUTH, SOUTHWEST, WEST, NORTHWEST };
enum EnumCoordinates { ONE_LATITUDE = 9, FARTHER_NORTH, EAST_OF, ONE_LONGITUDE, FARTHER_SOUTH, TO_THE_WEST };

struct City
{
    City() { this->number = 0; this->x = 0; this->y = 0; };
    City(int number, int x, int y) { this->number = number; this->x = x; this->y = y; };
    City& operator = (const City& other) { this->number = other.number; this->x = other.x; this->y = other.y; this->suitable = other.suitable; return *this; };
    bool suitable = false;
    int number, x, y;
    vector<int> nodes, roads;
};

int Latitude(City &from, City &to)
{
    if (from.y < to.y) // Цель севернее
    {
        return EnumCoordinates::FARTHER_NORTH;
    }
    else if (from.y > to.y) // Цель южнее
    {
        return EnumCoordinates::FARTHER_SOUTH;
    }
    else // Широта совпадает
    {
        return EnumCoordinates::ONE_LATITUDE;
    }
}
int Longitude(City &from, City &to)
{
    if (from.x > to.x) // Цель западнее
    {
        return EnumCoordinates::TO_THE_WEST;
    }
    else if (from.x < to.x) // Цель восточнее
    {
        return EnumCoordinates::EAST_OF;
    }
    else // Долгота совпадает
    {
        return EnumCoordinates::ONE_LONGITUDE;
    }
}
int Compass(City &from, City &to)
{
    int latitude = Latitude(from, to);
    int longitude = Longitude(from, to);
    if (latitude == EnumCoordinates::FARTHER_NORTH && longitude == EnumCoordinates::ONE_LONGITUDE) // Север
    {
        return EnumDirection::NORTH;
    }
    if (latitude == EnumCoordinates::FARTHER_NORTH && longitude == EnumCoordinates::EAST_OF) // Северовосток
    {
        return EnumDirection::NORTHEAST;
    }
    if (latitude == EnumCoordinates::ONE_LATITUDE && longitude == EnumCoordinates::EAST_OF) // Восток
    {
        return EnumDirection::EAST;
    }
    if (latitude == EnumCoordinates::FARTHER_SOUTH && longitude == EnumCoordinates::EAST_OF) // Юговосток
    {
        return EnumDirection::SOUTHEAST;
    }
    if (latitude == EnumCoordinates::FARTHER_SOUTH && longitude == EnumCoordinates::ONE_LONGITUDE) // Юго
    {
        return EnumDirection::SOUTH;
    }
    if (latitude == EnumCoordinates::FARTHER_SOUTH && longitude == EnumCoordinates::TO_THE_WEST) // Югозапад
    {
        return EnumDirection::SOUTHWEST;
    }
    if (latitude == EnumCoordinates::ONE_LATITUDE && longitude == EnumCoordinates::TO_THE_WEST) // Запад
    {
        return EnumDirection::WEST;
    }
    if (latitude == EnumCoordinates::FARTHER_NORTH && longitude == EnumCoordinates::TO_THE_WEST) // Северозапад
    {
        return EnumDirection::NORTHWEST;
    }
    return 0;
}
bool PassingDirection(int departure, int arrival)
{
    bool pass;
    if (departure == EnumDirection::NORTH) // Север
    {
        pass = (arrival == EnumDirection::NORTHWEST || arrival == EnumDirection::NORTH || arrival == EnumDirection::NORTHEAST) ? true : false;
    }
    else if (departure == EnumDirection::NORTHEAST) // Северовосток
    {
        pass = (arrival == EnumDirection::NORTH || arrival == EnumDirection::NORTHEAST || arrival == EnumDirection::EAST) ? true : false;
    }
    else if (departure == EnumDirection::EAST) // Восток
    {
        pass = (arrival == EnumDirection::NORTHEAST || arrival == EnumDirection::EAST || arrival == EnumDirection::SOUTHEAST) ? true : false;
    }
    else if (departure == EnumDirection::SOUTHEAST) // Юговосток
    {
        pass = (arrival == EnumDirection::EAST || arrival == EnumDirection::SOUTHEAST || arrival == EnumDirection::SOUTH) ? true : false;
    }
    else if (departure == EnumDirection::SOUTH) // Юг
    {
        pass = (arrival == EnumDirection::SOUTHEAST || arrival == EnumDirection::SOUTH || arrival == EnumDirection::SOUTHWEST) ? true : false;
    }
    else if (departure == EnumDirection::SOUTHWEST) // Югозапад
    {
        pass = (arrival == EnumDirection::SOUTH || arrival == EnumDirection::SOUTHWEST || arrival == EnumDirection::WEST) ? true : false;
    }
    else if (departure == EnumDirection::WEST) // Запад
    {
        pass = (arrival == EnumDirection::SOUTHWEST || arrival == EnumDirection::WEST || arrival == EnumDirection::NORTHWEST) ? true : false;
    }
    else
    {
        pass = (departure == arrival) ? true : false;
    }
    return pass;
}
int Distance(City &a, City &b)
{
    return abs(a.x - b.x) + abs(a.y - b.y);
}
void DiscardCitys(City &from, int dist, int direction, vector<City> &catalog)
{
    for (auto it = catalog.begin(); it != catalog.end(); ++it)
    {
        if (PassingDirection(direction, Compass(from, *it)))
        {
            if (Distance(from, *it) <= dist)
            {
                it->suitable = true;
            }
        }
    }
    auto startCity = catalog.begin();
    advance(startCity, from.number - 1);
    startCity->suitable = true;
}
void FillNodes(City &arrival, vector<City> &catalog, int power)
{
    for (auto &from : catalog)
    {
        if (from.suitable)
        {
            for (auto &to : catalog)
            {
                if (to.suitable)
                {
                    if (from.number == to.number)
                    {
                        continue;
                    }
                    else
                    {
                        if (Distance(from, to) <= power && PassingDirection(Compass(from, arrival), Compass(from, to))) // Дистанция и направление подходят
                        {
                            from.nodes.emplace_back(to.number);
                        }
                    }
                }
            }
        }
    }
}
int Pathfinding(City &departure, City &arrival, vector<City> &catalog, int power)
{
    FillNodes(arrival, catalog, power);
    stack<int> request;
    request.push(departure.number);
    while (true)
    {
        if (request.size() > 0)
        {
            auto from = catalog.begin();
            advance(from, request.top() - 1);
            request.pop();
            for (auto &nodesFrom : from->nodes)
            {
                auto to = catalog.begin();
                advance(to, nodesFrom - 1);
                request.push(to->number);
                if (from->roads.size() > 0)
                {
                    auto min = min_element(from->roads.begin(), from->roads.end());
                    to->roads.emplace_back(*min + 1);
                }
                else
                {
                    to->roads.emplace_back(1);
                }
                if (to->number == arrival.number)
                {
                    auto min = min_element(to->roads.begin(), to->roads.end());
                    if (*min == 1)
                    {
                        return 1;
                    }
                }
            }
        }
        else
        {
            break;
        }
    }
    if (arrival.roads.size() > 0)
    {
        auto min = min_element(arrival.roads.begin(), arrival.roads.end());
        return *min;
    }
    else
    {
        return -1;
    }
}

int main()
{
    
    // Инициализация
    int cities, powerReserve, startCity, finishCity;
    vector<City> citiesCatalog;
    cin >> cities;
    #ifdef FORMAT_ENTER
        for (int i = 1; i <= cities; i++)
        {
            int x, y;
            cin >> x;
            system("cls");
            cout << cities << endl;
            for (const auto& el : citiesCatalog)
            {
                cout << el.x << " " << el.y << endl;
            }
            cout << x << " ";
            cin >> y;
            City n(i, x, y);
            citiesCatalog.emplace_back(n);
            system("cls");
            cout << cities << endl;
            for (const auto& el : citiesCatalog)
            {
                cout << el.x << " " << el.y << endl;
            }
        }
        cin >> powerReserve >> startCity;
        system("cls");
        cout << cities << endl;
        for (const auto& el : citiesCatalog)
        {
            cout << el.x << " " << el.y << endl;
        }
        cout << powerReserve << endl << startCity << " ";
        cin >> finishCity;
        system("cls");
        cout << cities << endl;
        for (const auto& el : citiesCatalog)
        {
            cout << el.x << " " << el.y << endl;
        }
        cout << powerReserve << endl << startCity << " " << finishCity << endl;
    #else
        for (int i = 1; i <= cities; i++)
        {
            int x, y;
            cin >> x >> y;
            City n(i, x, y);
            citiesCatalog.emplace_back(n);
            
        }
        cin >> powerReserve >> startCity >> finishCity;
    #endif
    auto fromCity = citiesCatalog.begin();
    advance(fromCity, startCity - 1);
    auto toCity = citiesCatalog.begin();
    advance(toCity, finishCity - 1);
    // Расчет и вывод
    DiscardCitys(*fromCity, Distance(*fromCity, *toCity), Compass(*fromCity, *toCity), citiesCatalog);
    cout << Pathfinding(*fromCity, *toCity, citiesCatalog, powerReserve);
}