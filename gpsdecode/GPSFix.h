/*
 * GPSFix.h
 *
 *  Created on: Jul 23, 2014
 *      Author: Cameron Karlsson
 *
 *  See the license file included with this source.
 */

#ifndef GPSFIX_H_
#define GPSFIX_H_

#include <cstdint>
#include <ctime>
#include <string>
#include <chrono>
#include <vector>
#include <cmath>
#include <sstream>
#include <QString>
namespace nmea {

	class GPSSatellite;
	class GPSAlmanac;
	class GPSFix;
	class GPSService;


	// =========================== GPS SATELLITE =====================================

	class GPSSatellite {
	public:
		GPSSatellite() :
            idx(0),
            snr(0),
			elevation(0),
            azimuth(0)
		{};

		//satellite data
        int idx;
		double snr;			// 0-99 dB
        QString prn;		// id - 0-32
		double elevation;	// 0-90 deg
		double azimuth;		// 0-359 deg
		std::string toString();
		operator std::string();
	};






	// =========================== GPS ALMANAC =====================================
 typedef struct __ii{
        QString id;                                     //卫星编号
        int idx;                                        //卫星系统所以 0：GPS 1：北斗 2：GLONASS
 }II;

	class GPSAlmanac {
        friend class GPSService;
	private:
        uint32_t gsaPage;
		uint32_t visibleSize;
        uint32_t lastPageGP;
        uint32_t totalPagesGP;
        uint32_t lastPageGB;
        uint32_t totalPagesGB;
        uint32_t lastPageGL;
        uint32_t totalPagesGL;
        uint32_t processedPages;

	public:
		GPSAlmanac() :
            lastPageGP(0),
            totalPagesGP(0),
            lastPageGB(0),
            totalPagesGB(0),
            lastPageGL(0),
            totalPagesGL(0)
		{};

        void clear();			//will remove all information from the satellites

		//mapped by prn
		std::vector<GPSSatellite> satellites;
//        std::vector<GPSSatellite> satellitesGB;
//        std::vector<GPSSatellite> satellitesGP;
//        std::vector<GPSSatellite> satellitesGL;
        std::vector<II> satellitesT;
//        std::vector<int> satellitesTGB;
//        std::vector<int> satellitesTGP;
//        std::vector<int> satellitesTGL;
		double averageSNR();
		double minSNR();
		double maxSNR();

	};




	// =========================== GPS TIMESTAMP =====================================

	// UTC time
	class GPSTimestamp {
	private:
		std::string monthName(uint32_t index);
	public:
		GPSTimestamp();

		int32_t hour;
		int32_t min;
        int32_t sec;

		int32_t month;
		int32_t day;
		int32_t year;

		// Values collected directly from the GPS
		double rawTime;
		int32_t rawDate;

		time_t getTime();

		// Set directly from the NMEA time stamp
		// hhmmss.sss
		void setTime(double raw_ts);

		// Set directly from the NMEA date stamp
		// ddmmyy
		void setDate(int32_t raw_date);

		std::string toString();
	};






	// =========================== GPS FIX =====================================

    class GPSFix {
        friend class GPSService;

	private:

		bool haslock;
		bool setlock(bool b);		//returns true if lock status **changed***, false otherwise.


	public:

		GPSFix();
		virtual ~GPSFix();


		GPSAlmanac almanac;
		GPSTimestamp timestamp;

        char location;          //定位状态 A: 已定位 V: 未定位

		char status;		// Status: A=active, V=void (not locked)
		uint8_t type;		// Type: 1=none, 2=2d, 3=3d
		uint8_t quality;	// Quality: 
							//    0 = invalid
							//    1 = GPS fix (SPS)
							//    2 = DGPS fix
							//    3 = PPS fix
							//    4 = Real Time Kinematic (RTK)
							//    5 = Float RTK
							//    6 = estimated (dead reckoning) (2.3 feature)

		double dilution;					// Combination of Vertical & Horizontal
		double horizontalDilution;			// Horizontal dilution of precision, initialized to 100, best =1, worst = >20
		double verticalDilution;			// Vertical is less accurate

        double accuracy;

		double altitude;		// meters
		double latitude;		// degrees N
		double longitude;		// degrees E
		double speed;			// km/h
        double direct;
		double travelAngle;		// degrees true north (0-360)
		int32_t trackingSatellites;
		int32_t visibleSatellites;

		bool locked();
		double horizontalAccuracy();
		double verticalAccuracy();
		bool hasEstimate();
		
		std::chrono::seconds timeSinceLastUpdate();	// Returns seconds difference from last timestamp and right now.

		std::string toString();
		operator std::string();

		static std::string travelAngleToCompassDirection(double deg, bool abbrev = false);
	};

}

#endif /* GPSFIX_H_ */
