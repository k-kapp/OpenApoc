#include "city.h"
#include "building.h"
#include "organisation.h"
#include "buildingtile.h"
#include "framework/framework.h"
#include "game/resources/gamecore.h"
#include <random>

namespace OpenApoc {

City::City(Framework &fw, std::string mapName)
	: TileMap(fw, Vec3<int>{100, 100, 10}), organisations(Organisation::defaultOrganisations)
{
	auto file = fw.data->load_file("xcom3/ufodata/" + mapName, "rb");
	if (!file)
	{
		std::cerr << "Failed to open city map: " << mapName << "\n";
		return;
	}

	this->buildings = loadBuildingsFromBld(fw, mapName + ".bld", this->organisations, Building::defaultNames);
	this->cityTiles = CityTile::loadTilesFromFile(fw);

	for (int z = 0; z < this->size.z; z++)
	{
		for (int y = 0; y < this->size.y; y++)
		{
			for (int x = 0; x < this->size.x; x++)
			{
				uint16_t tileID;
				PHYSFS_readULE16(file, &tileID);
				if (tileID == -1 &&
				    PHYSFS_eof(file))
				{
					std::cerr << "Unexpected EOF reading citymap at x = " << x
						<< " y = " << y << " z = " << z << "\n";
					tileID = 0;
				}
				if (tileID)
				{
					Building *bld = nullptr;
					for (auto &b : this->buildings)
					{
						if (b.bounds.intersects(Vec2<int>{x,y}))
						{
							if (bld)
							{
								std::cerr << "Multiple buildings on tile at x = " << x
									<< " y = " << y << " z = " << z << "\n";
							}
							bld = &b;
						}
					}
					if (tileID < 0 ||
						tileID >= this->cityTiles.size())
					{
						std::cerr << "Invalid tile IDX " << tileID << " at x = " << x
							<< " y = " << y << " z = " << z << "\n";
					}
					else
					{
						this->tiles[z][y][x].objects.push_back(std::make_shared<BuildingSection>(&this->tiles[z][y][x], this->cityTiles[tileID], Vec3<int>{x,y,z}, bld));
					}
				}
			}
		}
	}

	std::default_random_engine generator;
	std::uniform_int_distribution<int> xydistribution(0,99);
	std::uniform_int_distribution<int> zdistribution(0,9);
	//Place 1000 random cars
	for (int i = 0; i < 1000; i++)
	{
		int x = 0;
		int y = 0;
		int z = 0;
		while (!this->tiles[z][y][x].objects.empty())
		{
			x = xydistribution(generator);
			y = xydistribution(generator);
			z = zdistribution(generator);
		}

		std::shared_ptr<Vehicle> testVehicle(fw.gamecore->vehicleFactory.create("POLICE_HOVERCAR"));
		this->vehicles.push_back(testVehicle);
		std::shared_ptr<FlyingVehicle> testVehicleObject(new FlyingVehicle(*testVehicle, &this->tiles[z][y][x]));
		testVehicle->tileObject = testVehicleObject;
		this->tiles[z][y][x].objects.push_back(testVehicleObject);
		//Vehicles are active
		this->activeObjects.push_back(testVehicleObject);
	}
	std::cerr << "Placed cars\n";

	std::cerr << "PATH TEST {0,0,9} to {99,99,9}\n";

	std::list<Tile*> path;
	path = this->findShortestPath(Vec3<int>{0,0,9}, Vec3<int>{99,99,9});

	std::cerr << "Route found in " << path.size() << " steps\n";
	for (auto tile : path)
	{
		std::cerr << "Tile {" << tile->position.x << "," << tile->position.y << "," << tile->position.z << "}\n";
	}

	PHYSFS_close(file);

}

City::~City()
{

}

}; //namespace OpenApoc
