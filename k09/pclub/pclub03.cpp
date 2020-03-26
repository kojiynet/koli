
/*
	
	pclub03.cpp
	
	pclub01.cppから派生。01は以下のもの。
	@Programming Club, Imaplus, Nov 16, 2019
	Instant Test for kdatasettest00.cpp
	
	Written by Koji Yamamoto
	Copyright (C) 2019-2020 Koji Yamamoto
	
	TODO:　
	度数分布表をつくる。kstatを見て。
	　別に、連続変数用の機能をつける。
	　　start/end, width, bin を指定する方式。
	　　自動で、スタージェスの公式を使う方式。
	　　階級の端点の表を与える方式。
	ヒストグラムを描く。
	SVGにする。
	
*/


/* ********** Preprocessor Directives ********** */

#include <k09/kdataset01.cpp>
#include <k09/kstat02.cpp>
#include <k09/koutputfile00.cpp>
#include <iostream> 
#include <iomanip>
#include <algorithm>

#include <boost/algorithm/string.hpp>


/* ********** Namespace Declarations/Directives ********** */

using namespace std;


/* ********** Class Declarations ********** */


/* ********** Enum Definitions ********** */


/* ********** Function Declarations ********** */

int main( int, char *[]);


/* ********** Class Definitions ********** */


/* ********** Global Variables ********** */


/* ********** Definitions of Static Member Variables ********** */


/* ********** Function Definitions ********** */

// ↓消す
/*
std::vector <double>
getGridPoints_Older( double min0, double max0)
{

	using namespace std;

	vector <double> ret;

	if ( min0 >= max0){ // error
		return ret;
	}

	double width = max0 - min0;

	// defining getInterval() 
	struct { 
		double operator()( double w)
		{

			double inte = w / 10.0; // 10個ぐらいの点を打つ。
			double digits_m1 = floor( log10( inte));
			double headdigit = ceil( inte / ( pow( 10.0, digits_m1))); // 切り上げた値の、先頭の1桁の数

			// headdigitが1か2か5ならそのまま。
			// それ以外なら、間隔を広げる方向に丸める。
			if ( headdigit <= 2.0){
				inte = headdigit * pow( 10.0, digits_m1);
			} else if ( headdigit <= 5){
				inte = 5.0 * pow( 10.0, digits_m1);
			} else {
				inte = pow( 10.0, digits_m1 + 1.0);
			}

			return inte;
			
		}
	} getInterval;

	// defining getPointsUsingBase()
	struct {
		void operator()( vector <double> &r, double b, double l0, double u0, double inte)
		{
			r.push_back( b); // 基準点。
			// 正の方向と負の方向に、intervalをとりながら、点を挙げていく。
			for ( double p = b + inte; p < u0; p += inte){
				r.push_back( p);
			}
			for ( double p = b - inte; p > l0; p -= inte){
				r.push_back( p);
			}

			sort( r.begin(), r.end());

		}
	} getPointsUsingBase; 

	if ( min0 * max0 <= 0.0){ // min0とmax0で符号が違うか、いずれかがゼロ

		double interval = getInterval( width);

		getPointsUsingBase( ret, 0.0, min0, max0, interval);

	} else { // min0とmax0で符号が同じ

		double abmin0 = abs( min0);
		double abmax0 = abs( max0);
		double mindigits_m1 = floor( log10( abmin0));
		double maxdigits_m1 = floor( log10( abmax0));
		
		if ( mindigits_m1 != maxdigits_m1){ // min0とmax0で桁数が異なる

			double interval = getInterval( width);

			double basepoint;

			if ( min0 > 0){ // 正の場合 
				basepoint = pow( 10.0, maxdigits_m1);
			} else { // 負の場合
				basepoint = -1.0 * pow( 10.0, mindigits_m1);
			}

			getPointsUsingBase( ret, basepoint, min0, max0, interval);
			
		} else { // min0とmax0で桁数が等しい

			// ここを書く。

		}


	}

	return ret;

}
*/

// [min0, max0]に、いい感じの間隔で点をとる。
// k0個以上で最小の点を返す。
std::vector <double>
getGridPoints( double min0, double max0, int k0 = 4)
{

	using namespace std;

	vector <double> ret;
	
	// この数以上の最小の点を返すようにする。
	int minnpoints = k0; 

	// error
	if ( min0 >= max0){ 
		alert( "getGripPoints()");
		return ret;
	}

	double max0ab = abs( max0);
	double min0ab = abs( min0);

	// max0abとmin0abのうち大きい方は何桁？（その値マイナス1）
	double digits_m1 = floor( log10( max( max0ab, min0ab)));

	// 基準となる10のべき乗値
	double base10val = pow( 10.0, digits_m1);

	// 候補となる、intervalの先頭の桁の値
	vector <double> headcands = { 5.0, 2.5, 2.0, 1.0};

	bool loop = true;
	while ( loop){

		for ( auto h : headcands){

			ret.clear();
			double interval = base10val * h;

			// setting startpoint; to avoid startpoint being "-0", we do a little trick.
			double startpoint = ceil( min0 / interval);
			if ( startpoint > -1.0 && startpoint < 1.0){
				startpoint = 0.0;
			}
			startpoint *= interval;

			for ( double p = startpoint; p <= max0; p += interval){
				ret.push_back( p);
			}
			if ( ret.size() >= minnpoints){
				loop = false;
				break;
			}

		}

		base10val /= 10.0;

	}

	return ret;

}

int main( int, char *[])
{
	
	vector <double> dvec;
	vector <double> dvecclean;

	{
		Dataset ds;
		bool b;

		cout << "Reading data...";
		b = ds.readCsvFile( "jhpsmerged_191029_v403.csv");
		if ( b == false){
			return 0;
		}
		cout << "Done." << endl;

		cout << "Fixing variable types...";
		int nnum, nmis;
		ds.fixVariableType( nnum, nmis);	
		cout << "Done." << endl;
		
		cout << "Getting numeric vector before specifying missing...";
		b = ds.getNumericVectorWithoutMissing( dvec, "v403");
		if ( b == false){
			return 0;
		}
		cout << "Done." << endl;
		
		cout << "Specifying missing cases...";
		ds.specifyValid( 
			"v403",
			[]( double v)->bool{ return ( v < 99999.0);}
		);
		cout << "Done." << endl;

		cout << "Getting numeric vector excl. missing...";
		b = ds.getNumericVectorWithoutMissing( dvecclean, "v403");
		if ( b == false){
			return 0;
		}
		cout << "Done." << endl;
		
	}

	cout << endl;
	cout << "***************************************************" << endl;
	cout << "JHPS 2009 Household Income incl. Tax" << endl;
	cout << "Calculated by mean() and median()" << endl;
	cout << "Mean:   " << setprecision( 15) << mean( dvecclean)   << " (Ten Thousand Yen)" << endl;
	cout << "Median: " << median( dvecclean) << " (Ten Thousand Yen)" << endl;
	cout << "***************************************************" << endl;
	cout << "FYI: Mean from \"dirty\" data: " << setprecision( 15) << mean( dvec) << endl;


	// 度数分布表

	cout << endl;
	cout << "Number of unique values: " << countUniqueValues( dvecclean) << endl;
	cout << "FYI Number of unique values in \"dirty\" vector: " << countUniqueValues( dvec) << endl << endl;


	RecodeTable <double, int> rt;
	rt.setAutoTableFromContVar( dvecclean); 

	cout << "RecodeTable:" << endl;
	rt.print( cout, ","); 
	cout << endl;

	FreqType <int, int> ft;
	ft.setFreqFromRecodeTable( dvecclean, rt);

	ft.printPadding( cout);


	// ヒストグラムをつくりたい。
	// SVGの座標系をいじるところから。
	// SVGのviewBoxについて：アスペクト比が違っているとわかりにくい。
	// （強制的に余白がつくられたりするか、強制的に拡大縮小して円が歪んだりする）ので、
	// svgタグのサイズとviewBoxのサイズを合わせたい。

	struct Cambus;
	struct Point;
	
	struct Point {

		double x, y;

		Point( void)
		 : x( std::numeric_limits<double>::quiet_NaN()),
		   y( std::numeric_limits<double>::quiet_NaN())
		{}
		
		Point( double x0, double y0) : x( x0), y( y0)
		{}

	};

	struct Cambus {

		// 実際の座標系では、y座標は大きいほど「下」の位置を示す。
		// 論理座標系では、y座標は大きいほど「上」の位置を示す。

		double actuXMin, actuYMin, actuXMax, actuYMax; // 実際の座標系での、枠の範囲
		double actuWidth, actuHeight; // 同上

		double theoXMin, theoYMin, theoXMax, theoYMax; // 論理座標系での、枠の範囲
		double theoWidth, theoHeight; // 同上

		void setTheoretical( double xmin0, double ymin0, double xmax0, double ymax0)
		{
			theoXMin = xmin0; theoYMin = ymin0; theoXMax = xmax0; theoYMax = ymax0; 
			theoWidth = xmax0 - xmin0; theoHeight = ymax0 - ymin0; 
		}

		void setActual( double xmin0, double ymin0, double xmax0, double ymax0)
		{
			actuXMin = xmin0; actuYMin = ymin0; actuXMax = xmax0; actuYMax = ymax0; 
			actuWidth = xmax0 - xmin0; actuHeight = ymax0 - ymin0; 
		}

		// 論理座標系表現から実際の座標系表現を作成。
		Point getActualFromTheoretical( const Point &poi0)
		{
			
			double x0 = poi0.x;
			double y0 = poi0.y;
			Point ret;
			ret.x = ( x0 - theoXMin) / theoWidth  * actuWidth  + actuXMin; 
			ret.y = ( theoYMax - y0) / theoHeight * actuHeight + actuYMin; 
			return ret;

		}

		// 実際の座標系での中点のxを返す。
		double getActualMidX( void)
		{

			return ( actuXMin + actuWidth / 2);

		}
	
	};

	koutputfile outsvg( "pclub03out.svg");
	outsvg.open( false, false, true);

	vector <string> svglines;

	vector <int> codes;
	vector <int> counts;
	vector <double> leftvec;
	vector <double> rightvec;
	ft.getVectors( codes, counts);
	ft.getRangeVectors( leftvec, rightvec);
	Cambus cam;

	// SVG領域の大きさと、座標系のある領域の大きさを指定することで、それらしく計算してほしい。

	svglines.push_back( R"(<?xml version="1.0" encoding="UTF-8" ?>)"); // This should be exactly in the first line.
	svglines.push_back( R"(<svg width="500px" height="500px" viewBox="0 0 500 500" xmlns="http://www.w3.org/2000/svg">)");
	svglines.push_back( R"(  <rect x="50" y="50" width="400" height="400" fill="Gainsboro" />)");

	cam.setActual( 50, 50, 450, 450);
	cam.setTheoretical( -100, -100, 10000, 1200);

	// ちょうどいい間隔のグリッド線をひく。
	// 同、軸の目盛を打つ。

	// ちょうどいい間隔と基準点の実験。
	{
		vector <double> gridpoints = getGridPoints( -12.34, 567.8);
		for ( auto d : gridpoints){
			cout << d << endl;
		}
		cout << endl;
	}
	
	{
		vector <double> gridpoints = getGridPoints( -1234.5, 567.8);
		for ( auto d : gridpoints){
			cout << d << endl;
		}
		cout << endl;
	}

	{
		vector <double> gridpoints = getGridPoints( -1234.5, 567.8, 5);
		for ( auto d : gridpoints){
			cout << d << endl;
		}
		cout << endl;
	}

	{
		vector <double> gridpoints = getGridPoints( 123.5, 5678.9);
		for ( auto d : gridpoints){
			cout << d << endl;
		}
		cout << endl;
	}

	{
		vector <double> gridpoints = getGridPoints( -80001.0, -299.9);
		for ( auto d : gridpoints){
			cout << d << endl;
		}
		cout << endl;
	}

	{
		vector <double> gridpoints = getGridPoints( -80001.0, -299.9, 5);
		for ( auto d : gridpoints){
			cout << d << endl;
		}
		cout << endl;
	}


	for ( int i = 0; i < codes.size(); i++){

		Point theoP1( leftvec[ i], counts[ i]); // left-top
		Point theoP2( rightvec[ i], 0); // right-bottom 
		Point actuP1 = cam.getActualFromTheoretical( theoP1);
		Point actuP2 = cam.getActualFromTheoretical( theoP2);

		stringstream ss;
		ss << "  "
		<< R"(<rect)"
		<< " "
		<< R"(x=")" << actuP1.x << R"(")"
		<< " "
	    << R"(y=")" << actuP1.y << R"(")"
		<< " "
		<< R"(width=")" << ( actuP2.x - actuP1.x) << R"(")"
		<< " "
		<< R"(height=")" << ( actuP2.y - actuP1.y) << R"(")"
		<< " "
		<< R"(stroke=")" << "Gray" << R"(")"
		<< " "
		<< R"(fill=")" << "Gray" << R"(")" 
		<< " "
		<< R"(/>)";
		
		svglines.push_back( ss.str());

	}

	string title = "Frequency from pclub03.cpp";

	// Title 
	{
		stringstream ss;
		double fontsize = std::floor( cam.actuWidth * 0.7 / title.size() * 2.0); // 描画領域の幅のうち、7割を占めるぐらいのサイズ
		if ( fontsize >= cam.actuYMin * 0.7){ // 余白の70%より大きいのはダメ
			fontsize = cam.actuYMin * 0.7;
		}
		ss << "  "
		<< R"(<text)"
		<< " "
		<< R"(x=")" << ( std::round( cam.getActualMidX())) << R"(")" // 中央揃えをするので。
		<< " "
		<< R"(y=")" << ( std::round( cam.actuYMin * 0.9)) << R"(")" // 余白のうち10%浮かせる。
		<< " "
		<< R"(font-family=")" << "Arial,san-serif" << R"(")"
		<< " "
		<< R"(font-size=")" << fontsize << R"(")" 
		<< " "
		<< R"(text-anchor="middle")"
		<< " "
		<< R"(dominant-baseline="text-after-edge")"
		<< ">"
		<< title 
		<< R"(</text>)";

		svglines.push_back( ss.str());
	}



	// 軸の目盛ラベルを
	// 軸タイトルを書く。


	svglines.push_back( R"(</svg>)");

	string detector = "<!-- " 
	                  u8"\u6587\u5B57\u30B3\u30FC\u30C9\u8B58\u5225\u7528" // 「文字コード識別用」というUTF-8文字列
	                  " -->";
	svglines.push_back( detector);

	outsvg.writeLines( svglines);

	outsvg.close();


	// 今のRecodeTableには、左端・右端がない（無限大）という指定ができない。

	// FreqTypeにはすごく小さい機能だけを持たせることにして、
	// 別にFreqTableTypeか何かをつくって、そこに、RecodeTableを持たせたり、
	// それをもとにしたFreqを作らせたりしてもよいかも。

	/*
	度数分布表をつくる。kstatを見て。
	　別に、連続変数用の機能をつける。
	　　start/end, width, bin を指定する方式。
	　　自動で、スタージェスの公式を使う方式？
	　　※Stataでは、min{ sqrt(N), 10*ln(N)/ln(10)}らしいので、それでいく。
	　　階級の端点の表を与える方式。
	*/

	return 0;

}


/* ********** Definitions of Member Functions ********** */

