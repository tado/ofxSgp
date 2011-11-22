//
//  ofxSgp.cpp
//
//
//  Created by mori koichiro on 11/10/13.
//  Copyright (c) 2011年 __MyCompanyName__. All rights reserved.
//
    
#include "ofMain.h"
#include "ofxSgp.h"

#include <stdio.h>
#include <string.h>

//--------------------------
ofxSgp::ofxSgp()
{
       
}

//--------------------------
ofxSgp::~ofxSgp()
{
    
}
//--------------------------
void ofxSgp::setup(const char * tleFilePath)
{
    eclipse_depth = 0;
    tle_file = tleFilePath;
    
//    cout<<str<<endl;
//    tle_file = ofToDataPath(tleFilePath);
//ここでファイルパス相対対応に
    flg = Input_Tle_Set(tle_file, &tle);
    
    if( flg == -1 )
    {
        printf(" File open failed - Exiting!\n");
        exit(-1);
    }
    
    /* Print satellite name and TLE read status */
    printf(" %s: ", tle.sat_name);
    if( flg == -2 )
    {
        printf("TLE set bad - Exiting!\n");
        exit(-2);
    }
    else
        printf("TLE set good - Happy Tracking!\n");
    
    ClearFlag(ALL_FLAGS);
    
    select_ephemeris(&tle);
}

//--------------------------
void ofxSgp::update()
{

    UTC_Calendar_Now(&utc);
    jul_utc = Julian_Date(&utc);
    
    jul_epoch = Julian_Date_of_Epoch(tle.epoch);
    tsince = (jul_utc - jul_epoch) * xmnpda;
    

    if( isFlagSet(DEEP_SPACE_EPHEM_FLAG) )
        strcpy(ephem,"SDP4");
    else
        strcpy(ephem,"SGP4");
    

    if( isFlagSet(DEEP_SPACE_EPHEM_FLAG) )
        SDP4(tsince, &tle, &pos, &vel);
    else
        SGP4(tsince, &tle, &pos, &vel);
    

    Convert_Sat_State( &pos, &vel );
    

    Magnitude( &vel );
    sat_vel = vel.w;
    

    Calculate_Obs(jul_utc, &pos, &vel, &obs_geodetic, &obs_set);
    
    Calculate_LatLonAlt(jul_utc, &pos, &sat_geodetic);
    
    
    Calculate_Solar_Position(jul_utc, &solar_vector);
    Calculate_Obs(jul_utc,&solar_vector,&zero_vector,&obs_geodetic,&solar_set);
    
    if( Sat_Eclipsed(&pos, &solar_vector, &eclipse_depth) )
        SetFlag( SAT_ECLIPSED_FLAG );
    else
        ClearFlag( SAT_ECLIPSED_FLAG );
    
    if( isFlagSet( SAT_ECLIPSED_FLAG ) )
        strcpy( sat_status, "Eclipsed" );
    else
        strcpy( sat_status, "In Sunlight" );
    
    /* Convert and print satellite and solar data */
    sat_azi = Degrees(obs_set.x);
    sat_ele = Degrees(obs_set.y);
    sat_range = obs_set.z;
    sat_range_rate = obs_set.w;
    sat_lat = Degrees(sat_geodetic.lat);
    sat_lon = Degrees(sat_geodetic.lon);
    sat_alt = sat_geodetic.alt;
    
    sun_azi = Degrees(solar_set.x);
    sun_ele = Degrees(solar_set.y);

}

//--------------------------
ofVec3f ofxSgp::getPos()
{
    ofVec3f posVec;
    //物理学の座標系とoFの座標系がちがう？
    posVec.set(pos.x,pos.z,pos.y);
    return posVec;
}

//--------------------------
ofVec3f ofxSgp::getVelocity()
{
    ofVec3f velVec;
    //物理学の座標系とoFの座標系がちがう？
    velVec.set(vel.x,vel.z,vel.y);
    return velVec;
}
    

