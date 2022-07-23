#pragma once

#include <weather_icons.h>

typedef struct
{
	uint16_t	code;
	const char* group;
	const char* description;
	const z_image_data_t* imageDay;
	const z_image_data_t* imageNight;
} weather_t;

const weather_t weather_types[] = 
{ 
	{	200,	"Thunderstorm",	"thunderstorm with light rain",		&id_200d,	&id_200n },
	{	201,	"Thunderstorm",	"thunderstorm with rain",			&id_201d,	&id_201n },
	{	202,	"Thunderstorm",	"thunderstorm with heavy rain",		&id_202d,	&id_202n },
	{	210,	"Thunderstorm",	"light thunderstorm",				&id_210d,	&id_210n },
	{	211,	"Thunderstorm",	"thunderstorm",						&id_211d,	&id_211n },
	{	212,	"Thunderstorm",	"heavy thunderstorm",				&id_212d,	&id_212n },
	{	221,	"Thunderstorm",	"ragged thunderstorm",				&id_221d,	&id_221n },
	{	230,	"Thunderstorm",	"thunderstorm with light drizzle",	&id_230d,	&id_230n },
	{	231,	"Thunderstorm",	"thunderstorm with drizzle",		&id_231d,	&id_231n },
	{	232,	"Thunderstorm",	"thunderstorm with heavy drizzle",	&id_232d,	&id_232n },
	{	300,	"Drizzle",		"light intensity drizzle",			&id_300d,	&id_300n },
	{	301,	"Drizzle",		"drizzle",							&id_301d,	&id_301n },
	{	302,	"Drizzle",		"heavy intensity drizzle",			&id_302d,	&id_302n },
	{	310,	"Drizzle",		"light intensity drizzle rain",		&id_310d,	&id_310n },
	{	311,	"Drizzle",		"drizzle rain",						&id_311d,	&id_311n },
	{	312,	"Drizzle",		"heavy intensity drizzle rain",		&id_312d,	&id_312n },
	{	313,	"Drizzle",		"shower rain and drizzle",			&id_313d,	&id_313n },
	{	314,	"Drizzle",		"heavy shower rain and drizzle",	&id_314d,	&id_314n },
	{	321,	"Drizzle",		"shower drizzle",					&id_321d,	&id_321n },
	{	500,	"Rain",			"light rain",						&id_500d,	&id_500n },
	{	501,	"Rain",			"moderate rain",					&id_501d,	&id_501n },
	{	502,	"Rain",			"heavy intensity rain",				&id_502d,	&id_502n },
	{	503,	"Rain",			"very heavy rain",					&id_503d,	&id_503n },
	{	504,	"Rain",			"extreme rain",						&id_504d,	&id_504n },
	{	511,	"Rain",			"freezing rain",					&id_511d,	&id_511n },
	{	520,	"Rain",			"light intensity shower rain",		&id_520d,	&id_520n },
	{	521,	"Rain",			"shower rain",						&id_521d,	&id_521n },
	{	522,	"Rain",			"heavy intensity shower rain",		&id_522d,	&id_522n },
	{	531,	"Rain",			"ragged shower rain",				&id_531d,	&id_531n },
	{	600,	"Snow",			"light snow",						&id_600d,	&id_600n },
	{	601,	"Snow",			"snow",								&id_601d,	&id_601n },
	{	602,	"Snow",			"heavy snow",						&id_602d,	&id_602n },
	{	611,	"Snow",			"sleet",							&id_611d,	&id_611n },
	{	612,	"Snow",			"light shower sleet",				&id_612d,	&id_612n },
	{	613,	"Snow",			"shower sleet",						&id_613d,	&id_613n },
	{	615,	"Snow",			"light rain and snow",				&id_615d,	&id_615n },
	{	616,	"Snow",			"rain and snow",					&id_616d,	&id_616n },
	{	620,	"Snow",			"light shower snow"	,				&id_620d,	&id_620n },
	{	621,	"Snow",			"shower snow",						&id_621d,	&id_621n },
	{	622,	"Snow",			"heavy shower snow",				&id_622d,	&id_622n },
	{	701,	"Mist",			"mist",								&id_701d,	&id_701n },
	{	711,	"Smoke",		"smoke",							&id_711d,	&id_711n },
	{	721,	"Haze",			"haze",								&id_721d,	&id_721n },
	{	731,	"Dust",			"sand/dust whirls",				 	&id_731d,	&id_731n },
	{	741,	"Fog",			"fog",	 							&id_741d,	&id_741n },
	{	751,	"Sand",			"sand",	 							&id_751d,	&id_751n },
	{	761,	"Dust",			"dust",			 					&id_761d,	&id_761n },
	{	762,	"Ash",			"volcanic ash",	 					&id_762d,	&id_762n },
	{	771,	"Squall",		"squalls",	 						&id_771d,	&id_771n },
	{	781,	"Tornado",		"tornado",	 						&id_781d,	&id_781n },
	{	800,	"Clear",		"clear sky",	   					&id_800d,	&id_800n },
	{	801,	"Clouds",		"few clouds: 11-25%",   			&id_801d,	&id_801n },
	{	802,	"Clouds",		"scattered clouds: 25-50%",		  	&id_802d,	&id_802n },
	{	803,	"Clouds",		"broken clouds: 51-84%",	  		&id_803d,	&id_803n },
	{	804,	"Clouds",		"overcast clouds: 85-100%",  		&id_804d,	&id_804n }
};

const weather_t* lookup_weather_code(const int code)
{
	for (auto const &id : weather_types)
		if (id.code == code)
			return &id;
	
	return nullptr;
}
