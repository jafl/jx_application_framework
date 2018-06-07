#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#define __USE_ISOC99 1
#include <math.h>
//#include <bits/nan.h>
#include <assert.h>

static int crash_divzero = 0, crash_assert = 0;

// This program tests for a variable that changes from a struct to a
// pointer and back again to see if the variable tree parses it correctly.

/*

python pretty printers:  http://sourceware.org/gdb/wiki/STLSupport

*/

struct teststruct
{
	int a;
	int b;

	void	foo(const int j) const { int i=1; i++; }
};

typedef void MyFunction(void);

template <class T> class MyTmpl
{
public:

	enum Color
	{
		red = 1,
		green,
		blue
	};

	MyTmpl();
};

struct Test
{
	MyFunction*	f;
	MyTmpl<int>::Color	color;

	void*	CompName;
	void*	InstName;
	void*	GenStr[25];
	long	In1Size, Out1Size, In2Size, Out2Size, In3Size, Out3Size;
	void*	InVec;
	void*	OutVec;
	void*	CompData;
	void*	SimData;
	void*	Function_Info;
};

class VkComponent { };
typedef void* Widget;
class ModeGraph;
class AModeList;
class AMode;
typedef int Cardinal;
typedef char* String;
typedef char Boolean;

struct Arg
{
	const char* name;
	int value;
};

struct UIAppDefault
{
	char* cName;
	char* wName;
	char* cInstName;
	char* wRsc;
	char* value;
};

struct VkCallbackStruct
{
	char* client_data;
	Widget obj;
};

struct AModeTab : public VkComponent
{
	static const char *const DisplayAMode;

	Widget _AModeTab;
	Widget _aModeTab;

	ModeGraph *		_graph;
	AModeList * 	_AModes;
	AMode *			_curAMode;

	Cardinal          ac;
	Arg               args[256];

	static String         _defaultAModeTabResources[1];
	static UIAppDefault   _appDefaults[1];
	static Boolean        _initAppDefaults;

	VkCallbackStruct  _default_callback_data;
};

const char *const AModeTab::DisplayAMode = "DisplayAMode";
String AModeTab::_defaultAModeTabResources[] = { nullptr };
UIAppDefault AModeTab::_appDefaults[] = {{ nullptr, nullptr, nullptr, nullptr, nullptr }};
Boolean AModeTab::_initAppDefaults = 0;

template <class T>
class Vec3
{
public:

	Vec3() { n[0] = 0; n[1] = 0; n[2] = 0; };

private:

	T n[3];
};

template <class T>
class Pt3 : public Vec3<T>
{
public:

	Pt3() { vertfcn = nullptr; vert2dfcn = nullptr; circfcn = nullptr; };
	virtual ~Pt3() {};

	static void (*vertfcn)( T x, T y, T z);
	static void (*vert2dfcn)( T, T);
	static void (*circfcn)( T x, T y, T rad);
};

template <class T> void (*Pt3<T>::vertfcn)( T x, T y, T z);
template <class T> void (*Pt3<T>::vert2dfcn)( T, T);
template <class T> void (*Pt3<T>::circfcn)( T x, T y, T rad);

template class Vec3 < double >;
template class Pt3 < double >;

#define MAX_ACID_LEN 39
#define MAX_ACTYPE_LEN 9
#define MAX_AC_DATA 8

struct VEC;
struct airport;
struct GDPLocation;

struct aircraft {
	 int     int_id;                  /* Unique integer aircraft id          */
	 char    id[MAX_ACID_LEN+1];      /* Aircraft id (airline + flight #     */
	 char    type[MAX_ACTYPE_LEN+1];  /* Aircraft type                       */
	 double   perf_coef_climb;        /* Aircraft type performance climb coef*/
	 double   perf_coef_descent;      /* Aircraft type performance dscnt coef*/
	 struct type_perf *performance;   /* Aircraft type performance data      */
	 char    status[MAX_AC_DATA];     /* Aircraft status (AC_FLYING, etc.)   */
	 double  track_altitude;          /* Flight Level from ETMS file         */
	 double  track_time;              /* time stamp from ETMS file.          */
	 double  track_lat;               /* updated track position in degrees   */
	 double  track_lng;               /* updated track position in degrees   */
	 double  track_hdg;               /* updated track heading in degrees    */
	 char    alt_type;                /* alt type from the most recent update.*/
	 double  altitude_curr[MAX_AC_DATA]; /* Current altitude.  This reflects *
						  * climb and descent, so it will vary; *
						  * but it should always be less that   *
						  * altitude_comm, which is the altitude*
						  * that an aircraft is cleared to      *
						  * cruise at.                          */
	 double  altitude_comm;          /* Commanded altitude for cruising     */
	 double  altitude_rate[MAX_AC_DATA];/* d(altitude)/dt in ft/min          */
	 double  track_gnd_spd;           /* gnd speed from ETMS file (kts)      */
	 double  ground_speed[MAX_AC_DATA];  /* Ground speed, knots              */
	 double  air_speed[MAX_AC_DATA];  /* Air speed, knots                    */
	 double   max_airspd_sec;         /* Maximum air speed, naut.miles/sec   */
	 double  heading[MAX_AC_DATA];    /* Heading (degrees)                   */
	 double  chi_d[MAX_AC_DATA];      /* desired course heading (no wind - deg.)*/
	 char    not_descending_yet[MAX_AC_DATA];  /* Flag if a/c descending yet */
	 int     destination_altitude;    /* Altitude of the destination         */
	 short   descent_leg;             /* Leg on which to start the descent   */
	 double   descent_range;          /* Distance from dest. to start descent*/
	 double   descent_leg_dist;       /* Distance down descent_leg to start  */
	 short   sim_markers;             /* Flag markers are from sim input src.*
					   * (sim input source is usually a file)*
					   * This will almost always be "true",  *
					   * until a user manually changes the   *
					   * flight plan.  Then it will be false.*/
	 short   num_markers;             /* Number of flight plan marker coords */
	 double  *marker_lat;             /* Latitude position of each marker    */
	 double  *marker_lng;             /* Longitude position of each marker   */
	 char   *marker_route;            /* Flight plan route string.  When     *
					   * parsed, the coordinates of each     *
					   * flight plan marker are placed into  *
					   * the marker_ arrays.  This string is *
					   * kept for comparison to see if the   *
					   * flight plan changes, so we know to  *
					   * update the marker_ arrays.          */
	 char    origin[MAX_ACID_LEN];    /* departure airport.                  */
	 char    destination[MAX_ACID_LEN];/* arrival airport.                   */
	 short   pred_num_markers;        /* number of fp wypts for predictions  */
	 double   *pred_marker_lat;       /* lat of fp wypts for predictions     */
	 double   *pred_marker_lng;       /* lng of fp wypts for predictions     */
	 double  tau_dot[MAX_AC_DATA];    /* d(tau)/dt in rad/sec                */
	 double  lambda_dot[MAX_AC_DATA]; /* d(lambda)/dt in rad/sec             */
	 double  time[MAX_AC_DATA];       /* Time stamp of this position         */
	 double  curr_lat[MAX_AC_DATA];   /* Latitude of the current position(deg)*/
	 double  curr_long[MAX_AC_DATA];  /* Longitude of the current position(deg)*/
	 int     curr_leg[MAX_AC_DATA];   /* Current leg of the journey          */
	 double   curr_projx[MAX_AC_DATA]; /* Projected x of current position     */
	 double   curr_projy[MAX_AC_DATA]; /* Projected y of current position     */
	 double   curr_projx_dot[MAX_AC_DATA]; /*x-dot in the stereo. graph. proj frame.*/
	 double   curr_projy_dot[MAX_AC_DATA]; /*y-dot in the stereo. graph. proj frame.*/
	 double  v_s[MAX_AC_DATA];        /* Stereographic Velocity Calc.        */
	 double  chi_s[MAX_AC_DATA];      /* Stereographic Heading Calc.         */
	 int     size_hist;               /* Size of the history arrays          */
	 int     num_hist;                /* Number of history records           */
	 double  *hist_lat;               /* History of aircraft's latitudes     */
	 double  *hist_lng;               /* History of aircraft's longitudes    */
	 double  *hist_projx;             /* History of aircraft's projected x   */
	 double  *hist_projy;             /* History of aircraft's projected y   */
	 int     *hist_alt;               /* History of aircraft's altitude      */
	 long    take_off_time;           /* Time an aircraft took off           */
	 short   compute_path;            /* Flag to compute the flight path.    *
					   * The flight path is what shows up on *
					   * screen when the user chooses the    *
					   * "show flight plan" option.          */
	 int     num_path;                /* Number of flight path records       *
					   * num_path >= num_markers and the     *
					   * coordinates in the path_ arrays are *
					   * a super set of the marker_ arrays.  *
					   * The extra points in the path_ arrays*
					   * reflect any curvature of a          *
					   * Great Circle path (if the leg is    *
					   * long enough to have any curve).     */
	 double  *path_lat;               /* Flight path latitudes               */
	 double  *path_lng;               /* Flight path longitudes              */
	 char   direct_to;		     /* Direct-To routing flag              */
	 char   first_dd;	     	     /* first dyn. density calc flag        */
	 char   future_dd;		     /* first predicted future dd value     */
	 char   conflict_found;           /* flag if a conflict was found        */
	 char   future_conflict_found;    /* flag if a future conflict was found */
	 int    last_time;
	 double last_alt;                 /* alt at previous time step           */
	 double last_heading;             /* heading at previous time step       */
	 double last_speed;               /* speed at previous time step         */
	 double last_future_alt;          /* future alt at previous time step    */
	 double last_future_heading;      /* future heading at previous time step*/
	 double last_future_speed;        /* future speed at previous time step  */
	 double last_future_time;
	 struct predicted_values *fp_pred;/* continuous fp traj prediction       */
	 struct predicted_values *gc_pred;/* continuous fp traj prediction       */
	 struct predicted_values *avoidCell;/* cell avoidance parameters.        */
	 struct predicted_values *noAvoid;  /* cell avoidance parameters.        */
	 struct aircraft *next;           /* Pointer to the next aircraft        */
	 int cdr_ac_num;                  /* "cdr" aircraft number               */
	 char cdr_change_speed;           /* "cdr" flag to change ground speed   */
	 char cdr_change_chi;             /* "cdr" flag to change heading angle  */
	 double cdr_speed;                /* "cdr" computed ground speed         */
	 double cdr_heading;              /* "cdr" computed heading              */
	 char cdr2_change_heading;        /* "cdr2" flag to change heading       */
	 char cdr2_change_speed;          /* "cdr2" flag to change speed         */
	 double cdr2_spd;                 /* "cdr2" computed speed               */
	 double cdr2_chi;                 /* "cdr2" computed heading             */
	 double unmodified_speed;         /* speed without applied resolution    */
	 double unmodified_heading;       /* heading without applied resolution  */
	 double path_length;              /* total flight path length            */
	 double path_time;                /* total flight path travel time       */
	 int    horiz_wthr_idx;           /* location on the wthr matrix grid.   */
	 int    vert_wthr_idx;            /* location on the wthr matrix grid.   */
	 char   intersects_wthr_cell;     /* wthr cell intersection flag         */
	 double distToCell;               /* dist to closest wthr cell.          */
	 char   metering_status;          /* meterings status for this ac.       */
	 double metering_spd;             /* speed of the metered ac. (kts)      */
	 char   copy_flag;                /* nonzero if aircraft copied to java  */
	 short  cur_sector;               /* current sector aircraft in (or -1)  */
	 short  cur_center;               /* current center aircraft in (or -1)  */
	 int    rerouteIdx;               /* index of reroute plan.              */
	 char   rerouteStatus;            /* reroute status of aircraft.         */
	 char   inFCA;                    /* flag if ac is in an FCA.            */
	 char   *currFCA;                 /* name of FCA the curr ac is in.      */
	 short  coast_time_steps;         /* number of time steps we've coasted  */
	 double entered_sector_time;      /* time plane entered cur_sector       */
	 struct AcMeterFix *metered_fixes;/* list of fixes to be metered at      */
	 int    first_pred_sector_index;  /* index of first sector pred. entry   */
	 int    last_pred_sector_index;   /* index of last sector pred. entry    */
	 int    orig_pred_airport_index;  /* index of orig airport pred. entry   */
	 int    dest_pred_airport_index;  /* index of dest airport pred. entry   */
	 char   recalc_prediction;        /* flag - prediction should be recalced*/
	 struct aircraft *waiting_ac;     /* in live data mode - this points to  *
					   *  an aircraft with the same ID that  *
					   *  is waiting for this one to land    *
					   *  before it can take off.            */
	 struct aircraft *arriving_ac;    /* in live data mode - this points to  *
					   *  an aircraft with the same ID that  *
					   *  must arrive before this aircraft   *
					   *  can take off.                      */
	 char   pred_can_fly;             /* flag - in live data mode - true if  *
					   *  pending arrival has landed, and    *
					   *  plane can now be flown.            */
	 double opt_dist;                 /* GC distance from org to dest        */
	 VEC    *s;                       /* s vector for wind-optimal calc.     */
	 struct airport *orig_airport;    /* originating airport                 */
	 struct airport *dest_airport;    /* destination airport                 */
	 int    eta;                      /* estimated time of arrival (sec.)    */
	 int    orig_pdt;                 /* orig proposed departure time (for GDP)*/
	 int    gdp_status;               /* ground delay/stop status            */
	 struct GDPLocation *gdp_loc;     /* GDP/GSP effecting this flight.      */
	 int    fp_eta;                   /* initial fp based eta (sec.)         */
	 struct aircraft** mMultiPosition;/* one aircraft may have alternate     *
					   * locations based on the following    *
					   *   miles in trail                    *
					   *   optimal miles in trail            *
					   *   regional metering                 */
};

void undef_f()
{
	int b=2;
	b++;

	if (crash_divzero)
		{
		int c=0;
		b /= c;
		}
	else if (crash_assert)
		{
		assert(0);
		}
}

void reference_struct_f()
{
	struct teststruct  b = { 1, 2 };
	struct teststruct& a = b;
	undef_f();
	b.foo(3);
}

void reference_value_f()
{
	int b  = 2;
	int& a = b;
	reference_struct_f();
}

void reference_null_f()
{
	int* b = 0;
	int& a = *b;
	reference_value_f();
}

void pointer_struct_f()
{
	struct teststruct  b = { 1, 2 };
	struct teststruct* a = &b;
	reference_null_f();
}

void pointer_array_f(int arg)
{
	int b[] = { 0, 1, 2 };
	int* a  = b;
	pointer_struct_f();
}

void pointer_value_f(int* arg)
{
	int  b = 2;
	int* a = &b;
	pointer_array_f(b);
}

void pointer_null_f()
{
	int* a = 0;
	pointer_value_f(a);
}

void spiral_for_plot_f()
{
	double x[100], y[100];
	memset(x, 0, 100*sizeof(double));
	memset(y, 0, 100*sizeof(double));

	for (int i=0; i<100; i++)
		{
		const double t = i * 10.0 * 3.141592654 / 180.0;
		const double r = i / 10.0;
		x[i] = r * cos(t);
		y[i] = r * sin(t);
		}

	pointer_null_f();
}

void array_of_array_f(struct teststruct arg[])
{
	int a[3][5] =
		{
		{  1,  2,  3,  4,  5 },
		{  6,  7,  8,  9, 10 },
		{ 11, 12, 13, 14, 15 }
		};
	spiral_for_plot_f();
}

void array_of_struct_f(struct teststruct& arg)
{
	struct teststruct a[] = {{ 1, 2 }, { 3, 4 }, { 5, 6 }, { 7, 8 }};
	array_of_array_f(a);
}

void struct_f(int arg[])
{
	struct teststruct a = { 1, 2 };
	array_of_struct_f(a);
}

void array_f(const char* arg)
{
	int a[] = { 0, 1, 2, 3, 4 };
	struct_f(a);
}

void string_f(float arg)
{
	const char* a = "this is a string aaaaaaaaaaaaaaaaab";
	array_f(a);
}

void value_f(Test* test, Pt3<double>& pt, aircraft& arg)
{
	float a = 3.2;
	string_f(a);
}

int main(int argc, char** argv)
{
	std::cout << "Hello World!" << std::endl;

	if (argc > 1 && strcmp(argv[1], "divzero") == 0)
		{
		crash_divzero = 1;
		}
	else if (argc > 1 && strcmp(argv[1], "assert") == 0)
		{
		crash_assert = 1;
		}

	Test* test = (Test*) calloc(1, sizeof(Test));
	test->f = undef_f;
	test->color = MyTmpl<int>::red;

	AModeTab* tab  = (AModeTab*) calloc(1, sizeof(AModeTab));
	tab->_AModeTab = (Widget) 0x819d080;
	tab->_aModeTab = (Widget) 0x819d258;
	tab->_graph    = (ModeGraph*) 0x8182698;
	tab->_AModes   = (AModeList*) 0x8138c68;
	tab->_curAMode = nullptr;
	tab->ac        = 2;
	tab->args[0].name  = "name";
	tab->args[0].value = 135926720;
	tab->args[1].name  = "itemCount";
	tab->args[1].value = 18;
	tab->_default_callback_data.client_data = nullptr;
	tab->_default_callback_data.obj = (Widget) 0x819c7d8;

	Pt3<double> v1;
	Pt3<double>* v2 = new Pt3<double>;

	std::vector<Pt3<double> > vec;
	vec.assign(3, v1);

	aircraft a;
	a.int_id = 0;
	memcpy(a.id, "111593908\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000", MAX_ACID_LEN+1);
	memcpy(a.type, "MD80\000\000\000\000\000\000", MAX_ACTYPE_LEN+1);
	a.perf_coef_climb = 0;
	a.perf_coef_descent = 0;
	a.performance = 0x0;
	memcpy(a.status, "\001\001\001\001\001\001\001\001", MAX_AC_DATA);
	a.track_altitude = 600;
	a.track_time = -999999.90000000002;
	a.track_lat = -999999.90000000002;
	a.track_lng = -999999.90000000002;
	a.track_hdg = -999999.90000000002;
	a.alt_type = '\000';
//	a.altitude_curr = {600, 0, 0, 0, 0, 0, 0, 0};
	a.altitude_comm = 600;
//	a.altitude_rate = {5.092789898267247e-313, 0, 0, 0, 0, 0, 0, 0};
	a.track_gnd_spd = 188;
//	a.ground_speed = {188, 0, 0, 0, 0, 0, 0, 0};
//	a.air_speed = {188, 0, 0, 0, 0, 0, 0, 0};
	a.max_airspd_sec = 0.1388888888888889;
//	a.heading = {188, 0, 0, 0, 0, 0, 0, 0};
	a.chi_d[0] = NAN;		// 1.1670976850259591e-312;
	a.chi_d[1] = -NAN;		// -nan(0xfffff00000037);
	a.chi_d[2] = INFINITY;	// 1.2095376008452646e-312;
	a.chi_d[3] = -INFINITY;	// 0xfffff00000039;
//	a.chi_d[4] = 0xfffff0000003a;
	a.chi_d[5] = 1.2731974745742227e-312;
//	a.chi_d[6] = 0xfffffffffffff;
	a.chi_d[7] = 1.2944174327852555e-312;
	memcpy(a.not_descending_yet, "\001\001\001\001\001\001\001\001", MAX_AC_DATA);
	a.destination_altitude = 0;
	a.descent_leg = 0;
	a.descent_range = 0;
	a.descent_leg_dist = 0;
	a.sim_markers = 1;
	a.num_markers = 6;
	a.marker_lat = 0x0;
	a.marker_lng = 0x0;
	a.marker_route = 0x0;
	memcpy(a.origin, "NOT_SET\0000ÀU@0ÀU@/Dialog\000\030\000\000\000\030\000\000\000java/awt", MAX_ACID_LEN);
	memcpy(a.destination, "NOT_SET\0000\000\000\000\030\000\000\000java/awt/ContainH\000\000\000\030\000\000", MAX_ACID_LEN);
	a.pred_num_markers = 0;
	a.pred_marker_lat = 0x0;
	a.pred_marker_lng = 0x0;
//	a.tau_dot = {-nan(0xfffffffffffff), 1.6763766752479363e-312, -nan(0xfffffffffffff), 1.7188165910771231e-312, 1.7612565064962354e-312, -nan(0xfffffffffffff), -nan(0xfffff00000053), 1.8036964223155408e-312};
//	a.lambda_dot = {-nan(0xfffff00000055), 1.8461363381348463e-312, -nan(0xfffff00000057), -nan(0xfffff00000058), 1.9097962118638044e-312, -nan(0xfffffffffffff), 1.9310161702230569e-312, -nan(0xfffffffffffff)};
//	a.time = {43212, 0, 0, 0, 0, 0, 0, 0};
//	a.curr_lat = {41.979444444444447, 0, 0, 0, 0, 0, 0, 0};
//	a.curr_long = {272.0958333333333, 0, 0, 0, 0, 0, 0, 0};
//	a.curr_leg = {0, 0, 0, 0, 0, 0, 0, 0};
//	a.curr_projx = {295.604240489294, -99999, -99999, -99999, -99999, -99999, -99999, -99999};
//	a.curr_projy = {310.0923767093272, -99999, -99999, -99999, -99999, -99999, -99999, -99999};
//	a.curr_projx_dot = {-999999.90000000002, -999999.90000000002, -999999.90000000002, -999999.90000000002, -999999.90000000002, -999999.90000000002, -999999.90000000002, -999999.90000000002};
//	a.curr_projy_dot = {-999999.90000000002, -999999.90000000002, -999999.90000000002, -999999.90000000002, -999999.90000000002, -999999.90000000002, -999999.90000000002, -999999.90000000002};
//	a.v_s = {2.1219957909652723e-314, -nan(0xffffc00000002), 0, 2.1219957904712067e-314, 0, 6.671360708370277e-316, 2.1219957909652723e-314, 3.0556739398743696e-312};
//	a.chi_s = {1.9762625833649862e-323, 6.7181716749422799e-270, 4.9406564584124654e-324, 6.671360708370277e-316, 2.1219957909652723e-314, -nan(0xffffc00000003), 0, 2.1219957904712067e-314};
	a.size_hist = 0;
	a.num_hist = 0;
	a.hist_lat = 0x0;
	a.hist_lng = 0x0;
	a.hist_projx = 0x0;
	a.hist_projy = 0x0;
	a.hist_alt = 0x0;
	a.take_off_time = 43212;
	a.compute_path = 0;
	a.num_path = 0;
	a.path_lat = 0x0;
	a.path_lng = 0x0;
	a.direct_to = '\000';
	a.first_dd = '\000';
	a.future_dd = '\000';
	a.conflict_found = '\000';
	a.future_conflict_found = '\000';
	a.last_time = -999999;
	a.last_alt = -999999.90000000002;
	a.last_heading = -999999.90000000002;
	a.last_speed = -999999.90000000002;
	a.last_future_alt = -999999.90000000002;
	a.last_future_heading = -999999.90000000002;
	a.last_future_speed = -999999.90000000002;
	a.last_future_time = 0;
	a.fp_pred = 0x0;
	a.gc_pred = 0x0;
	a.avoidCell = 0x0;
	a.noAvoid = 0x0;
	a.next = 0x0;
	a.cdr_ac_num = 909652841;
	a.cdr_change_speed = '\000';
	a.cdr_change_chi = '\000';
	a.cdr_speed = 4.7740243286082819e+180;
	a.cdr_heading = 6.7202220980884354e+194;
	a.cdr2_change_heading = '\000';
	a.cdr2_change_speed = '\000';
	a.cdr2_spd = 5.8705013557054259e-310;
	a.cdr2_chi = 87.041030963659523;
	a.unmodified_speed = -999999.90000000002;
	a.unmodified_heading = -999999.90000000002;
	a.path_length = 0;
	a.path_time = 0;
	a.horiz_wthr_idx = -1;
	a.vert_wthr_idx = -1;
	a.intersects_wthr_cell = '\000';
	a.distToCell = 9.8813129168249309e-324;
	a.metering_status = '\001';
	a.metering_spd = 0;
	a.copy_flag = 47;
	a.cur_sector = 29793;
	a.cur_center = -1;
	a.rerouteIdx = -1;
	a.rerouteStatus = -1;
	a.inFCA = -1;
	a.currFCA = 0x0;
	a.coast_time_steps = 0;
	a.entered_sector_time = 5.8263638432533482e-310;
	a.metered_fixes = (AcMeterFix*) 0x4055c2a0;
	a.first_pred_sector_index = 1079362208;
	a.last_pred_sector_index = 0;
	a.orig_pred_airport_index = 1248464696;
	a.dest_pred_airport_index = 79691840;
	a.recalc_prediction = '\000';
	a.waiting_ac = 0x0;
	a.arriving_ac = 0x0;
	a.pred_can_fly = '\001';
	a.opt_dist = 4.6896106981666495e-312;
	a.s = 0x0;
	a.orig_airport = 0x0;
	a.dest_airport = 0x0;
	a.eta = -999999;
	a.orig_pdt = -999999;
	a.gdp_status = -1;
	a.gdp_loc = 0x0;
	a.fp_eta = -999999;
	a.mMultiPosition = 0x0;

	std::map< int, std::string > myMap;
	myMap[5] = std::string("abc");
	myMap[2] = std::string("xyz");

	value_f(test, v1, a);

	free(test);
	return 0;
}
