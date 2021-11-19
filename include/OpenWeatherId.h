#pragma once

#include <image.h>

typedef struct
{
	uint16_t	id;
	const char* main;
	const char* description;
	const image_t* imageDay;
	const image_t* imageNight;
} OpenWeatherIdt;

const OpenWeatherIdt openWeatherIds[] = 
{ 
	{	200,	"Thunderstorm",	"thunderstorm with light rain",		&image_11,	&image_11	},
	{	201,	"Thunderstorm",	"thunderstorm with rain",			&image_11,	&image_11	},
	{	202,	"Thunderstorm",	"thunderstorm with heavy rain",		&image_11,	&image_11	},
	{	210,	"Thunderstorm",	"light thunderstorm",				&image_11,	&image_11	},
	{	211,	"Thunderstorm",	"thunderstorm",						&image_11,	&image_11	},
	{	212,	"Thunderstorm",	"heavy thunderstorm",				&image_11,	&image_11	},
	{	221,	"Thunderstorm",	"ragged thunderstorm",				&image_11,	&image_11	},
	{	230,	"Thunderstorm",	"thunderstorm with light drizzle",	&image_11,	&image_11	},
	{	231,	"Thunderstorm",	"thunderstorm with drizzle",		&image_11,	&image_11	},
	{	232,	"Thunderstorm",	"thunderstorm with heavy drizzle",	&image_11,	&image_11	},
	{	300,	"Drizzle",		"light intensity drizzle",			&image_09,	&image_09	},
	{	301,	"Drizzle",		"drizzle",							&image_09,	&image_09	},
	{	302,	"Drizzle",		"heavy intensity drizzle",			&image_09,	&image_09	},
	{	310,	"Drizzle",		"light intensity drizzle rain",		&image_09,	&image_09	},
	{	311,	"Drizzle",		"drizzle rain",						&image_09,	&image_09	},
	{	312,	"Drizzle",		"heavy intensity drizzle rain",		&image_09,	&image_09	},
	{	313,	"Drizzle",		"shower rain and drizzle",			&image_09,	&image_09	},
	{	314,	"Drizzle",		"heavy shower rain and drizzle",	&image_09,	&image_09	},
	{	321,	"Drizzle",		"shower drizzle",					&image_09,	&image_09	},
	{	500,	"Rain",			"light rain",						&image_10d,	&image_10n	},
	{	501,	"Rain",			"moderate rain",					&image_10d,	&image_10n	},
	{	502,	"Rain",			"heavy intensity rain",				&image_10d,	&image_10n	},
	{	503,	"Rain",			"very heavy rain",					&image_10d,	&image_10n	},
	{	504,	"Rain",			"extreme rain",						&image_10d,	&image_10n	},
	{	511,	"Rain",			"freezing rain",					&image_13,	&image_13	},
	{	520,	"Rain",			"light intensity shower rain",		&image_09,	&image_09	},
	{	521,	"Rain",			"shower rain",						&image_09,	&image_09	},
	{	522,	"Rain",			"heavy intensity shower rain",		&image_09,	&image_09	},
	{	531,	"Rain",			"ragged shower rain",				&image_09,	&image_09	},
	{	600,	"Snow",			"light snow",						&image_13,	&image_13	},
	{	601,	"Snow",			"snow",								&image_13,	&image_13	},
	{	602,	"Snow",			"heavy snow",						&image_13,	&image_13	},
	{	611,	"Snow",			"sleet",							&image_13,	&image_13	},
	{	612,	"Snow",			"light shower sleet",				&image_13,	&image_13	},
	{	613,	"Snow",			"shower sleet",						&image_13,	&image_13	},
	{	615,	"Snow",			"light rain and snow",				&image_13,	&image_13	},
	{	616,	"Snow",			"rain and snow",					&image_13,	&image_13	},
	{	620,	"Snow",			"light shower snow"	,				&image_13,	&image_13	},
	{	621,	"Snow",			"shower snow",						&image_13,	&image_13	},
	{	622,	"Snow",			"heavy shower snow",				&image_13,	&image_13	},
	{	701,	"Mist",			"mist",								&image_50,	&image_50	},
	{	711,	"Smoke",		"smoke",							&image_50,	&image_50	},
	{	721,	"Haze",			"haze",								&image_50,	&image_50	},
	{	731,	"Dust",			"sand/dust whirls",				 	&image_50,	&image_50	},
	{	741,	"Fog",			"fog",	 							&image_50,	&image_50	},
	{	751,	"Sand",			"sand",	 							&image_50,	&image_50	},
	{	761,	"Dust",			"dust",			 					&image_50,	&image_50	},
	{	762,	"Ash",			"volcanic ash",	 					&image_50,	&image_50	},
	{	771,	"Squall",		"squalls",	 						&image_50,	&image_50	},
	{	781,	"Tornado",		"tornado",	 						&image_50,	&image_50	},
	{	800,	"Clear",		"clear sky",	   					&image_01d,	&image_01n	},
	{	801,	"Clouds",		"few clouds: 11-25%",   			&image_02d,	&image_02n	},
	{	802,	"Clouds",		"scattered clouds: 25-50%",		  	&image_03d,	&image_03n	},
	{	803,	"Clouds",		"broken clouds: 51-84%",	  		&image_04,	&image_04	},
	{	804,	"Clouds",		"overcast clouds: 85-100%",  		&image_04,	&image_04	},
	{	0,		"Unknown",		"unknown",							nullptr,	nullptr		}
};
