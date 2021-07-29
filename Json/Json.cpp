#include <iostream>
#include <fstream>
#include "rapidjson/document.h"

using namespace rapidjson;
using namespace std;

int main()
{
	const char* json = "{\"results\":\"eyJ0eXAiOiJKV1QiLCJhbGciOiJIUzI1NiJ9.eyJleHAiOjE2Mjg3Nzg2ODEsImlhdCI6MTYyNzQ4MjY4MSwic2NvcGUiOiJleGNoYW5nZV9yYXRlIiwicGVybWlzc2lvbiI6MH0.3ivfrzWIUkxOgH5x0zYxqTjPq2Bd0AjhXDRSpK6s - zI\"}";
	Document d;
	d.Parse(json);

	Value& a = d["results"];
	cout << a.GetString();
	return 0;
}
