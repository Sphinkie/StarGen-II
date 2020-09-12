/* ****************************************************************************************
 * STARGEN II demo
 *
 * This sample program demonstrates the use of the StarGen II library.
 * It generates an XML and a TXT file.
 *
 * run:
 *    stargen.exe             to generate datasheets for a random solar system
 *    stargen.exe 0           to generate datasheets for our Solar System
 *    stargen.exe 97649       (or any number <120000) to generate a solar system based on the Celestia database.
 *
 * **************************************************************************************** */

#include <iostream>
#include <string>
#include <ctime>
#include "lib/SG_SolarSystem.h"
#include "lib/CE_Star.h"
#include "lib/CE_Catalog.h"

using namespace std;

int main(int argv, char **args)
{
    // You need this data on the star: Mass, Age, Seed (for random items)
    double mass;          // unit = Solar mass
    long double age;      // unit = years
    long double boloMagn; // Optional
    long seed;
    enum {help, sol_system, random_system, catalog_system} action;

    cout << "StarGen-II v2.1" << endl;
    cout << "by David de Lorenzo - 2020" << endl;

    if (argv == 2)
    {

        if (strcmp(args[1],"0")==0) action = sol_system ;
        else if (strcmp(args[1],"/?")==0) action = help;
        else if (strcmp(args[1],"-?")==0) action = help;
        else if (strcmp(args[1],"/h")==0) action = help;
        else if (strcmp(args[1],"-h")==0) action = help;
        else action = catalog_system;
        cout << endl;
    }
    else
    {
        action = random_system;
        cout << "run stargen /? for help" << endl;
        cout << endl;
    }

    switch (action)
    {
    // --------------------------------------------------------
    // SHOW HELP
    // --------------------------------------------------------
    case help:
        {
            cout << "help:" << endl;
            cout << " stargen .......... generates a datasheet for a random solar-system" << endl;
            cout << " stargen 0 ........ generates the datasheet of our Solar System" << endl;
            cout << " stargen 97649 .... (or any number <120000) generates a solar-system based on the Celestia database" << endl;
            cout << " stargen /? ....... shows this help message" << endl;
            cout<<"Press ENTER to continue....."<<endl;
            cin.ignore(1);
            break;
        }
    // --------------------------------------------------------
    // SOLAR SYSTEM
    // --------------------------------------------------------
    case sol_system:
        {
            cout << "Generating SOL system (StarGen model)" << endl;
            SG_SolarSystem* SG = new SG_SolarSystem(0);
            SG->setStarName("Sol");
            // This will generate a better "Sol" system
            // (force 10 planets: solid-solid-solid-solid-astero-gaz-gaz-gaz-gaz-solid)
            SG->generateSolarSystem("system_0.xml");
            SG->generateSolarSystem("system_0.txt");
            delete(SG);
            cout << "done." << endl;
            break;
        }
    // --------------------------------------------------------
    // We use the CELESTIA CATALOG
    // --------------------------------------------------------
    case catalog_system:
        {
            long CatNo = atol(args[1]);
            cout << "Generating solar-system from CELESTIA Catalog Index " << CatNo << endl;

            SG_SolarSystem* SG = new SG_SolarSystem(CatNo);  // we use the Catalog Number as a seed
            CE_Catalog* celestiaCatalog = new CE_Catalog("stars.dat");
            celestiaCatalog->readStars();  // you can set a limit. ex: 2000
            CE_Star* primaryStar = celestiaCatalog->getStar(CatNo);

            if (primaryStar==nullptr)
            {
                cout << "Index " << CatNo << " not in catalog" << endl;
                return 1;
            }
            mass     = primaryStar->getMass();
            boloMagn = primaryStar->getBolometricMagnitude();
            age      = primaryStar->getAge();
            string starName = primaryStar->getName();
            if (!starName.empty())
                cout << "star name is " << starName << endl;

            SG->setStarName(starName);
            SG->setStarMass(mass);  // Mass defines: Luminosity + Ecosphere + Life time of the star
            SG->setStarAge(age);

            /* Luminosity:
             * Luminosity is normally calculated from Mass.
             * But if you known the exact value, you can set it like this: */
            // SG->setStarLuminosity(3.5);    // (unit = Solar Luminosity)
            /* If you prefer, you can calculate the Luminosity, based on Bolometric Magnitude (all radiations) */
            // SG->setStarBoloMagnitude(-5);

            delete(celestiaCatalog);

            SG->generateSystem("system_" + to_string(CatNo) + ".xml");
            SG->generateSystem("system_" + to_string(CatNo) + ".txt");
            delete(SG);
            cout << "done." << endl;
            break;
        }
    // --------------------------------------------------------
    // Empty parameter or other cases: we use RANDOM values.
    // --------------------------------------------------------
    case random_system:
        {
        srand((int)time(0));           // initiate the random generator
        seed = (rand() % 100000) + 1;  // between 1 and 100.000
        cout << "Generating random solar-system from random seed " << seed << endl;
        SG_SolarSystem* SG = new SG_SolarSystem(seed);
        SG->setStarName("");              // will be autogenerated
        SG->setStarMass(RANDOM);
        SG->setStarAge(RANDOM);

        SG->generateSystem("system_" + to_string(seed) + ".xml");
        SG->generateSystem("system_" + to_string(seed) + ".txt");
        delete(SG);
        cout << "done." << endl;
        cout<<"Press ENTER to continue....."<<endl;
        cin.ignore(1);
        break;
        }
    }


    return 0;
}
