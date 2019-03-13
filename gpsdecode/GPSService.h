/*
 * GPSService.h
 *
 *  Created on: Aug 14, 2014
 *      Author: Cameron Karlsson
 *
 *  See the license file included with this source.
 */

#ifndef GPSSERVICE_H_
#define GPSSERVICE_H_

#include <string>
#include <chrono>
#include <functional>
#include <gpsdecode/GPSFix.h>
#include <gpsdecode/NMEAParser.h>
#include <gpsdecode/Event.h>

namespace nmea {

class GPSService {
private:
    QString getSBARSPrn(uint32_t src);
	void read_PSRF150(const NMEASentence& nmea);
    void read_GPGGA	(const NMEASentence& nmea);
    void read_GPGSA	(const NMEASentence& nmea); //GPS

	void read_GPGSV	(const NMEASentence& nmea);
	void read_GPRMC	(const NMEASentence& nmea);
	void read_GPVTG	(const NMEASentence& nmea);

    void read_GNGSA (const NMEASentence& nmea); //GPS + 北斗 + GLonass

    void read_GBGSV(const NMEASentence& nmea);  //北斗
    void read_GBGSA(const NMEASentence& nmea);

//    void read_GNGSA (const NMEASentence& nmea);
//    void read_GBGSV (const NMEASentence& nmea);
//    void read_GLGSV (const NMEASentence& nmea);

public:
	GPSFix fix;

	GPSService(NMEAParser& parser);
	virtual ~GPSService();

	Event<void(bool)> onLockStateChanged;		// user assignable handler, called whenever lock changes
	Event<void()> onUpdate;						// user assignable handler, called whenever fix changes

	void attachToParser(NMEAParser& parser);			// will attach to this parser's nmea sentence events
};


}

#endif /* GPSSERVICE_H_ */
