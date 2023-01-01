#include <iostream>
#include <iomanip>
#include <string>
#include <functional>
#include <algorithm> 
#include <vector>
#include <unordered_map> //mmm... constant time complexity search...

using namespace std; // Oh boy clean, legible code.  No namespace polution here boss.

// These are all just data we're not doing things to, so structs are appropriate
// With data oriented design we could colocate these in memory and make it very fast
struct Commodity {
	public:
		string name;
		float basePrice;
};

struct CommodityOddity {
	public :
		string name;	// Is likely an ID of some sort instead
		float demandPriceModifier;	// >1 = high demand, <1 = low demand
};


struct Location {
	public:
		string name;
		vector<CommodityOddity> commodityOddities;
};

unordered_map<string,Commodity> commodityList;

void initializecommodityList() {
	Commodity DistilledSpirits;
	DistilledSpirits.name = "Distilled Spirits";
	DistilledSpirits.basePrice = 5.0f;
	Commodity MedicalSupplies;
	MedicalSupplies.name = "Medical Supplies";
	MedicalSupplies.basePrice = 17.0f;
	Commodity ProcessedFoods;
	ProcessedFoods.name = "Processed Foods";
	ProcessedFoods.basePrice = 1.35f;

	commodityList = {
		{"Distilled Spirits", DistilledSpirits},
		{"Medical Supplies", MedicalSupplies},
		{"Processed Foods", ProcessedFoods}
	};
}


vector<Location> getLocationData() {
	// Normally I assume these are server calls, or maybe some mix of local and server.

	CommodityOddity BountifulFood = { "Processed Foods", 0.89f };
	CommodityOddity BountifulDrink = { "Distilled Spirits", 0.81f };
	CommodityOddity NeedsFoodBadly = { "Processed Foods", 1.16f };
	CommodityOddity NeedsFood = { "Processed Foods", 1.15f };
	CommodityOddity HasHealing = { "Medical Supplies", 0.93f };
	CommodityOddity NeedsHealing = { "Medical Supplies", 1.14f };

	Location GalleteFamilyFarms = { "Gallete Family Farms", {BountifulDrink, BountifulFood} };
	Location BountifulHarvestHydroponics = { "Bountiful Harvest Hydroponics", {BountifulFood} };
	Location ShadyGlenFarms = { "Shady Glen Farms", {BountifulFood} };
	Location BaijiniPoint = { "Baijini Point", {NeedsFoodBadly} };
	Location Area18 = { "Area 18", {NeedsFood} };
	Location DeakinsResearchOutpost = { "Deakins Research Outpost", {HasHealing} };
	Location HickesResearchOutpost = { "Hickes Research Outpost", {HasHealing} };
	Location RayariMcGrathResearchOutpost = { "Rayari McGrath Research Outpost", {HasHealing} };
	Location AmbitiousDreamStation = { "CRU-L1 Ambitious Dream Station", {NeedsHealing} };
	Location ShallowFieldsStation = { "CRU-L4 Shallow Fields Station", {NeedsHealing} };
	Location BeautifulGlenStation = { "CRU-L5 Beautiful Glen Station", {NeedsHealing} };
	
	vector<Location> compiledvector = {
		GalleteFamilyFarms,
		BountifulHarvestHydroponics,
		ShadyGlenFarms,
		BaijiniPoint,
		Area18,
		DeakinsResearchOutpost,
		HickesResearchOutpost,
		RayariMcGrathResearchOutpost,
		AmbitiousDreamStation,
		ShallowFieldsStation,
		BeautifulGlenStation
	};

	return compiledvector;
}

// Based on how it's displayed now, we're likely polling each place individually
void OldNBusted(vector<Location>& locationData) {
	vector<Location>::iterator currentLocation;
	vector<Location>::iterator finalLocation = locationData.end();
	for (currentLocation = locationData.begin(); currentLocation != finalLocation; ++currentLocation) {
		cout
			<< endl
			<< endl
			<< "Location: " + currentLocation->name;
		vector<CommodityOddity>::iterator currentOddity;
		vector<CommodityOddity>::iterator finalOddity = currentLocation->commodityOddities.end();
		for (currentOddity = currentLocation->commodityOddities.begin(); currentOddity != finalOddity; ++currentOddity) {
			bool isInDemand = currentOddity->demandPriceModifier > 1;
			string overUnder = isInDemand ? "Under" : "Over";
			string sellPurchase = isInDemand ? "Purchase" : "Sell";
			float finalPrice = commodityList[currentOddity->name].basePrice * currentOddity->demandPriceModifier;
			cout
				<< endl
				<< currentOddity->name << " - " << overUnder << "stock " << sellPurchase << " Price: " << finalPrice << " aUEC";
		}
	}
}

struct LocationPrice {
	string locationName;
	float finalPrice;
	string currencyType = "aUEC";
};

struct CommodityStock {
	public:
		vector<LocationPrice> overstockLocations;
		vector<LocationPrice> understockLocations;
};

// A little work need be done to get all the data first then display afterwards
// We want to get all the data once instead of making repeated calls
// to systems we don't want to overload
void NewHottness(vector<Location>& locationData) {
	unordered_map<string, CommodityStock> commodityStocks;

	vector<Location>::iterator currentLocation;
	vector<Location>::iterator finalLocation = locationData.end();
	for (currentLocation = locationData.begin(); currentLocation != finalLocation; ++currentLocation) {
		vector<CommodityOddity>::iterator currentOddity;
		vector<CommodityOddity>::iterator finalOddity = currentLocation->commodityOddities.end();
		for (currentOddity = currentLocation->commodityOddities.begin(); currentOddity != finalOddity; ++currentOddity) {
			string commodityName = currentOddity->name;
			bool isInDemand = currentOddity->demandPriceModifier > 1;
			float finalPrice = commodityList[commodityName].basePrice * currentOddity->demandPriceModifier;

			LocationPrice locationPrice = { currentLocation->name, finalPrice };

			if (commodityStocks.count(commodityName) < 1) {
				CommodityStock commodityStock;
				commodityStocks.emplace(commodityName, commodityStock);
			}

			if (isInDemand)
				commodityStocks[commodityName].understockLocations.push_back(locationPrice);
			else
				commodityStocks[commodityName].overstockLocations.push_back(locationPrice);
		}
	}

	unordered_map<string,CommodityStock>::iterator commodityStock;
	unordered_map<string, CommodityStock>::iterator listEnd = commodityStocks.end();
	cout << endl;
	for (commodityStock = commodityStocks.begin(); commodityStock != listEnd; ++commodityStock) {
		string commodityName = commodityStock->first;

		if (commodityStocks.count(commodityName) < 1) continue; // Don't list a commodity if its normal everywhere

		string commodityNameUpper = commodityName;

		// The things you take for granted working in garbage collected languages, right?
		transform(commodityNameUpper.begin(), commodityNameUpper.end(), commodityNameUpper.begin(), ptr_fun<int, int>(toupper));

		cout << endl
			<< "--- " << commodityNameUpper << " ---";

		// Overstocks
		vector<LocationPrice>::iterator locationPrice;
		vector<LocationPrice>::iterator finalLocation = commodityStocks[commodityName].overstockLocations.end();
		if (!commodityStocks[commodityName].overstockLocations.empty())
			cout << endl
				<< "OVERSTOCK";
		for (locationPrice = commodityStocks[commodityName].overstockLocations.begin(); locationPrice != finalLocation; ++locationPrice) {
			cout << endl
				<< "    *" << locationPrice->locationName << " @ " << locationPrice->finalPrice << " " << locationPrice->currencyType;
		}

		// Understocks
		if (!commodityStocks[commodityName].understockLocations.empty())
			cout << endl
				<< "UNDERSTOCK";
		finalLocation = commodityStocks[commodityName].understockLocations.end();
		for (locationPrice = commodityStocks[commodityName].understockLocations.begin(); locationPrice != finalLocation; ++locationPrice) {
			cout << endl
				<< "    *" << locationPrice->locationName << " @ " << locationPrice->finalPrice << " " << locationPrice->currencyType;
		}

		cout << endl
			<< "=========================================================================";
	}
}

int main()
{
	initializecommodityList();
	vector<Location> locationData = getLocationData();

	if (locationData.size() < 1) {
		cout << "No suitable data found.  Issuing data runner bounty.";
	}

	// Rounds aUEC to cents
	cout << fixed;
	cout << setprecision(2);

	cout << "CIG Iteration 3" << endl;

	cout << "The following have been issued adjusted commodity price based on their current supply levels:";
	
	OldNBusted(locationData);

	cout << endl << endl << "u/Z0MGbies proposed style" << endl;

	cout << "The following have been issued adjusted commodity price based on their current supply levels:";

	NewHottness(locationData);
}