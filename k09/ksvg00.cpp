
/*
	
	ksvg00.cpp
	Ver. k09.00
	
	Written by Koji Yamamoto
	Copyright (C) 2019-2020 Koji Yamamoto
	
*/


/* ********** Preprocessor Directives ********** */

#include <vector>
#include <string>
#include <sstream>

#include <k09/koutputfile00.cpp>


/* ********** Namespace Declarations/Directives ********** */

//using namespace std;


/* ********** Class Declarations ********** */

struct Cambus;
struct Point;

class SvgFile;
class SvgRect;
class SvgCircle;
class SvgEllipse; 
class SvgLine;
class SvgText;
//class SvgGroup;

class SvgGraph;


/* ********** Enum Definitions ********** */


/* ********** Function Declarations ********** */

SvgGraph createHistogram(
	const std::vector <double> &, const std::vector <double> &,
	const std::vector <int> &,
	bool = false
);

std::vector <double>
getGridPoints( double, double, int = 4, bool = true, bool = true);


/* ********** Class Definitions ********** */

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

	// x座標のみを算出→論理座標系表現から実際の座標系表現を作成。
	double getXActualFromTheoretical( double x0)
	{

		double retx = ( x0 - theoXMin) / theoWidth  * actuWidth  + actuXMin; 
		return retx;

	}

	// y座標のみを算出→論理座標系表現から実際の座標系表現を作成。
	double getYActualFromTheoretical( double y0)
	{
		
		double rety = ( theoYMax - y0) / theoHeight * actuHeight + actuYMin; 
		return rety;

	}

	// 実際の座標系での中点のxを返す。
	double getActualMidX( void)
	{

		return ( actuXMin + actuWidth / 2);

	}

	// 実際の座標系での中点のyを返す。
	double getActualMidY( void)
	{

		return ( actuYMin + actuHeight / 2);

	}

};

class SvgFile {

private:

	double width;
	double height;
	double viewBoxX1; 
	double viewBoxY1; 
	double viewBoxX2; 
	double viewBoxY2; 

	std::vector <std::string> filecontent;

	// This line should be exactly in the first line in SVG file.
	const std::string headline1   
	 = { R"(<?xml version="1.0" encoding="UTF-8" ?>)"};

public:

	SvgFile() = delete;

	SvgFile( double w0, double h0, double x1, double y1, double x2, double y2)
	 : width( w0), height( h0),
	   viewBoxX1( x1), viewBoxY1( y1), viewBoxX2( x2), viewBoxY2( y2),
	   filecontent()
	{}

	~SvgFile( void){}

	// これは最終的に他のメソッドで代替される予定。
	void addFileContent( const std::string &s0){
		filecontent.push_back( s0);
	}

	// defined later because of dependency problem
	void addRect( const SvgRect &); 
	void addCircle( const SvgCircle &); 
	void addEllipse( const SvgEllipse &); 
	void addLine( const SvgLine &); 
	void addText( const SvgText &);

/*
	これは優先順位が低い。
	void addGroup( const SvgGroup &);
*/
	std::vector <std::string> getFileContent( void)
	{
		std::vector <std::string> ret;
		ret.reserve( filecontent.size() + 4);

		// 1行目
		ret.push_back( headline1);

		// 2行目
		{
			std::stringstream ss;
			ss << R"(<svg width=")" << width << R"(px")" << " " 
			<< R"(height=")" << height << R"(px")" << " " 
			<< R"(viewBox=")" << viewBoxX1 << " " << viewBoxY1 << " " << viewBoxX2 << " " << viewBoxY2 << R"(")" << " " 
			<< R"(xmlns="http://www.w3.org/2000/svg">)";
			ret.push_back( ss.str());
		}

		// 3行目以降＝SVGの中身
		ret.insert( ret.end(), filecontent.begin(), filecontent.end());

		// 最後の2行
		ret.push_back( "</svg>");
		std::string detector = "<!-- " 
		                  u8"\u6587\u5B57\u30B3\u30FC\u30C9\u8B58\u5225\u7528" // 「文字コード識別用」というUTF-8文字列
		                  " -->";
		ret.push_back( detector);
		
		return ret;

	}

	// ファイルに書き出すメソッド
	// 正常終了でtrue、異常終了でfalseを返す。
	// 
	// appendのデフォルト値はtrue、overwriteのデフォルト値はfalse、askのデフォルト値はtrue。
	// 同じファイル名のファイルがすでに存在する場合の挙動は以下のとおり。
	// appendがtrueなら、追加書き込みモードで開く。overwriteとaskは無視する。
	// appendがfalseで、overwriteがtrueなら、上書きする。
	// appendがfalseで、overwriteがfalseで、askがtrueなら、上書きするかどうかをコンソールで尋ねる。
	// appendがfalseで、overwriteがfalseで、askがfalseなら、ファイルを開かない。
	bool writeFile( const std::string &fn0, bool append = false, bool overwrite = false, bool ask = true)
	{

		std::vector <std::string> svglines = getFileContent();
		koutputfile outsvg( fn0);

		bool ret = outsvg.open( append, overwrite, ask);
		if ( ret == false){
			return false;
		}

		ret = outsvg.writeLines( svglines);
		if ( ret == false){
			return false;
		}

		outsvg.close();
		return true;

	}

};

// 各属性をデータとして保有するのではなく属性指定テキストにして保有する。
class SvgRect {

private:
	
	std::vector <std::string> attrvec;
	std::vector <std::string> animatevec;

public:

	SvgRect( double x0, double y0, double w0, double h0)
	 : attrvec(), animatevec()
	{
		std::stringstream ss;
		ss << R"(x=")"      << x0 << R"(")" << " "
		   << R"(y=")"      << y0 << R"(")" << " "
		   << R"(width=")"  << w0 << R"(")" << " " 
		   << R"(height=")" << h0 << R"(")";
		attrvec.push_back( ss.str());
	}

	~SvgRect( void){}

	void addFill( const std::string &s0)
	{
		attrvec.push_back( R"(fill=")" + s0 + R"(")");
	}

	void addStroke( const std::string &s0)
	{
		attrvec.push_back( R"(stroke=")" + s0 + R"(")");
	}

	void addStrokewidth( double v0)
	{
		std::stringstream ss;
		ss << R"(stroke-width=")" << v0 << R"(")";
		attrvec.push_back( ss.str());
	}

	void addFillopacity( double v0)
	{
		std::stringstream ss;
		ss << R"(fill-opacity=")" << v0 << R"(")";
		attrvec.push_back( ss.str());
	}

	void addAnimate( const std::string &attr, double v1, double v2, double sec)
	{
		std::stringstream ss;
		ss << "<animate "
		   << "attributeName=\"" << attr << "\"" << " "
		   << "begin=\"0s\"" << " " 
		   << "dur=\"" << sec << "\"" << " "
		   << "from=\"" << v1 << "\"" << " " 
		   << "to=\"" << v2 << "\"" << " " 
		   << "repeatCount=\"1\""
		   << "/>";
		animatevec.push_back( ss.str());
	}

	std::string getContent( void) const
	{

		std::string ret;

		if ( animatevec.size() < 1){
		
			ret = "<rect";
			for ( const auto &attr : attrvec){
				ret += " ";
				ret += attr;
			}
			ret += " />";
		
		} else {
		
			ret = "<rect";
			for ( const auto &attr : attrvec){
				ret += " ";
				ret += attr;
			}
			ret += " >";
			for ( const auto &animatetag : animatevec){
				ret += animatetag;
			}
			ret += "</rect>";
		
		}

		return ret;

	}

};

// 各属性をデータとして保有するのではなく属性指定テキストにして保有する。
class SvgCircle {

private:
	
	std::vector <std::string> attrvec;

public:

	SvgCircle( double x, double y, double r)
	 : attrvec()
	{
		std::stringstream ss;
		ss << R"(cx=")" << x << R"(")" << " "
		   << R"(cy=")" << y << R"(")" << " "
		   << R"(r=")" << r << R"(")";
		attrvec.push_back( ss.str());
	}

	~SvgCircle( void){}

	void addFill( const std::string &s0)
	{
		attrvec.push_back( R"(fill=")" + s0 + R"(")");
	}

	void addStroke( const std::string &s0)
	{
		attrvec.push_back( R"(stroke=")" + s0 + R"(")");
	}

	void addStrokewidth( double v0)
	{
		std::stringstream ss;
		ss << R"(stroke-width=")" << v0 << R"(")";
		attrvec.push_back( ss.str());
	}

	std::string getContent( void) const
	{
		std::string ret = "<circle";
		for ( const auto &attr : attrvec){
			ret += " ";
			ret += attr;
		}
		ret += " />";
		return ret;
	}

};


// 各属性をデータとして保有するのではなく属性指定テキストにして保有する。
class SvgEllipse {

private:
	
	std::vector <std::string> attrvec;

public:

	SvgEllipse( double cx, double cy, double rx, double ry)
	 : attrvec()
	{
		std::stringstream ss;
		ss << R"(cx=")" << cx << R"(")" << " "
		   << R"(cy=")" << cy << R"(")" << " "
		   << R"(rx=")" << rx << R"(")" << " "
		   << R"(ry=")" << ry << R"(")";
		attrvec.push_back( ss.str());
	}

	~SvgEllipse( void){}

	void addFill( const std::string &s0)
	{
		attrvec.push_back( R"(fill=")" + s0 + R"(")");
	}

	void addStroke( const std::string &s0)
	{
		attrvec.push_back( R"(stroke=")" + s0 + R"(")");
	}

	void addStrokewidth( double v0)
	{
		std::stringstream ss;
		ss << R"(stroke-width=")" << v0 << R"(")";
		attrvec.push_back( ss.str());
	}

	std::string getContent( void) const
	{
		std::string ret = "<ellipse";
		for ( const auto &attr : attrvec){
			ret += " ";
			ret += attr;
		}
		ret += " />";
		return ret;
	}

};

// 各属性をデータとして保有するのではなく属性指定テキストにして保有する。
class SvgLine {

private:
	
	std::vector <std::string> attrvec;

public:

	SvgLine( double x1, double y1, double x2, double y2)
	 : attrvec()
	{
		std::stringstream ss;
		ss << R"(x1=")" << x1 << R"(")" << " "
		   << R"(y1=")" << y1 << R"(")" << " "
		   << R"(x2=")" << x2 << R"(")" << " "
		   << R"(y2=")" << y2 << R"(")";
		attrvec.push_back( ss.str());
	}

	~SvgLine( void){}

	void addFill( const std::string &s0)
	{
		attrvec.push_back( R"(fill=")" + s0 + R"(")");
	}

	void addStroke( const std::string &s0)
	{
		attrvec.push_back( R"(stroke=")" + s0 + R"(")");
	}

	void addStrokewidth( double v0)
	{
		std::stringstream ss;
		ss << R"(stroke-width=")" << v0 << R"(")";
		attrvec.push_back( ss.str());
	}

	std::string getContent( void) const
	{
		std::string ret = "<line";
		for ( const auto &attr : attrvec){
			ret += " ";
			ret += attr;
		}
		ret += " />";
		return ret;
	}

};

// 各属性をデータとして保有するのではなく属性指定テキストにして保有する。
class SvgText {

private:
	
	std::string text; 
	std::vector <std::string> attrvec;

public:

	SvgText( double x0, double y0, const std::string &s0)
	 : text( s0), attrvec()
	{
		std::stringstream ss;
		ss << R"(x=")" << x0 << R"(")" << " "
		   << R"(y=")" << y0 << R"(")";
		attrvec.push_back( ss.str());
	}

	SvgText( double x0, double y0, double v0)
	 : text(), attrvec()
	{
		{
			std::stringstream ss;
			ss << v0;
			text.assign( ss.str());
		}
		{
			std::stringstream ss;
			ss << R"(x=")" << x0 << R"(")" << " "
			   << R"(y=")" << y0 << R"(")";
			attrvec.push_back( ss.str());
		}
	}

	~SvgText( void){}

	void addFontfamily( const std::string &s0)
	{
		attrvec.push_back( R"(font-family=")" + s0 + R"(")");
	}	

	void addFontsize( double v0)
	{
		std::stringstream ss;
		ss << v0;
		attrvec.push_back( R"(font-size=")" + ss.str() + R"(")");
	}	

	void addTextanchor( const std::string &s0)
	{
		attrvec.push_back( R"(text-anchor=")" + s0 + R"(")");
	}	

	void addDominantbaseline( const std::string &s0)
	{
		attrvec.push_back( R"(dominant-baseline=")" + s0 + R"(")");
	}	

	void addRotate( double d0, double x0, double y0)
	{
		std::stringstream ss;
		ss << R"(transform="rotate)" << "(" << d0 << " " << x0 << " " << y0 << ")" << R"(")";
		attrvec.push_back( ss.str());
	}	

	std::string getContent( void) const
	{
		std::string ret = "<text";
		for ( const auto &attr : attrvec){
			ret += " ";
			ret += attr;
		}
		ret += ">";
		ret += text;
		ret += "</text>";
		return ret;
	}

};

/*
以下未実装
class SvgGroup {} ;
*/



class SvgGraph { 

private:

	SvgFile svgf;
	Cambus cam;
	double svgwidth;  // width of the whole SVG
	double svgheight; // height of the whole SVG

public:

	SvgGraph(
		double w0, double h0, double x1, double y1, double x2, double y2
	)
	 : svgf( w0, h0, x1, y1, x2, y2), svgwidth( w0), svgheight( h0)
	{}

	void setCambus( const Cambus &);

	void setBackground( const std::string &);

	void setGraphPaneColor( const std::string &);

	void drawXGridLines( 
		const std::vector <double> &, const std::string &
	);

	void drawYGridLines( 
		const std::vector <double> &, const std::string &
	);

	void drawBins(
		const std::vector <double> &,
		const std::vector <double> &,
		const std::vector <int> &,
		const std::string &,
		bool = false
	);

	void drawPoints(
		const std::vector <double> &,
		const std::vector <double> &,
		const std::string &
	);

	void drawCircle(
		double,
		double, 
		double,
		const std::string &
	);

	void drawXAxisTicks( 
		const std::vector <double> &, double, const std::string &
	);

	void drawYAxisTicks( 
		const std::vector <double> &, double, const std::string &
	);

	void setXAxisLabels(
		const std::vector <double> &, const std::string &, double, double, double
	);

	void setYAxisLabels(
		const std::vector <double> &, const std::string &, double, double, double
	);

	void setGraphTitle(
		const std::string &, const std::string &, double, double, double
	);

	void setXAxisTitle(
		const std::string &, const std::string &, double, double, double
	);

	void setYAxisTitle( 
		const std::string &, const std::string &, double, double, double
	);

	void drawGraphPaneFrame( const std::string);
	
	void addRectActu( const SvgRect &); 
	void addLineActu( const SvgLine &); 

	bool writeFile( const std::string &);

}; 


/* ********** Global Variables ********** */


/* ********** Definitions of Static Member Variables ********** */


/* ********** Function Definitions ********** */

SvgGraph createHistogram(
	const std::vector <double> &leftvec,
	const std::vector <double> &rightvec,
	const std::vector <int> &counts,
	bool animated /*= false*/
)
{

	std::string graph_title = "Frequency - restricted to v less than 2500";
	std::string xaxis_title = "Household Income";
	std::string yaxis_title = "#Cases";

	// SVG領域の大きさと、座標系のある領域の大きさを指定することで、それらしく計算してほしい。
	double svgwidth = 500;
	double svgheight = 500;
	double outermargin = 20;
	double graph_title_fontsize = 20;
	double graph_title_margin = 10; // グラフタイトルの下のマージン
	double axis_title_fontsize = 20;
	double axis_title_margin = 5; // 軸タイトルと軸ラベルの間のマージン
	double axis_label_fontsize = 14;
	double axis_ticklength = 5;

	SvgGraph svgg( svgwidth, svgheight, 0, 0, svgwidth, svgheight);

	// グラフ描画領域の座標（左上、右下）
	double graphpane_actuX1 = outermargin + axis_title_fontsize + axis_title_margin + axis_label_fontsize + axis_ticklength; 
	double graphpane_actuY1 = outermargin + graph_title_fontsize + graph_title_margin;
	double graphpane_actuX2 = svgwidth - outermargin; 
	double graphpane_actuY2 = svgheight - ( outermargin + axis_title_fontsize + axis_title_margin + axis_label_fontsize + axis_ticklength);

	double graphwidth = graphpane_actuX2 - graphpane_actuX1;
	double graphheight = graphpane_actuY2 - graphpane_actuY1;

	// ちょうどいい間隔のグリッド線の点と、範囲を得る。

	// x軸
	double xminval = leftvec.front();
	double xmaxval = rightvec.back();
	std::vector <double> xgridpoints = getGridPoints( xminval, xmaxval);
	/*
	for ( auto d : xgridpoints){
		std::cout << d << std::endl;
	}
	std::cout << std::endl;
	*/

	// y軸
	double yminval = 0;
	double ymaxval = *( max_element( counts.begin(), counts.end()));
	std::vector <double> ygridpoints = getGridPoints( yminval, ymaxval);
	/*
	for ( auto d : ygridpoints){
		std::cout << d << std::endl;
	}
	std::cout << std::endl;
	*/
	
	// 描画範囲は、Gridpointsのさらに5%外側にする。
	double theoWidthTemp = xgridpoints.back() - xgridpoints.front();
	double theoXMin = xgridpoints.front() - 0.05 * theoWidthTemp;
	double theoXMax = xgridpoints.back() + 0.05 * theoWidthTemp;
	
	double theoHeightTemp = ygridpoints.back() - ygridpoints.front();
	double theoYMin = ygridpoints.front() - 0.05 * theoHeightTemp;
	double theoYMax = ygridpoints.back() + 0.05 * theoHeightTemp;
	
	// SvgGraphインスタンス内のキャンバスの設定
	// 注：このCambusオブジェクトが与えられたあとで、このオブジェクトを用いて
	// 　　「即時に」座標変換がなされる。
	// 　　あとでCambusオブジェクトを入れ替えてもそれまでに追加された描画部品には影響しない。

	Cambus cam;
	cam.setActual( graphpane_actuX1, graphpane_actuY1, graphpane_actuX2, graphpane_actuY2);
	cam.setTheoretical( theoXMin, theoYMin, theoXMax, theoYMax);
	svgg.setCambus( cam);


	// 背景の描画開始

	svgg.setBackground( "whitesmoke");

	svgg.setGraphPaneColor( "gainsboro");

	svgg.drawXGridLines( xgridpoints, "silver");
	svgg.drawYGridLines( ygridpoints, "silver");

	// 背景の描画終了



	// メインの情報の描画終了
	if ( animated == true){
		svgg.drawBins( leftvec, rightvec, counts, "gray", true);
	} else {
		svgg.drawBins( leftvec, rightvec, counts, "gray");
	}

	// メインの情報の描画終了


	// 周辺情報記載の開始

	svgg.drawXAxisTicks( xgridpoints, axis_ticklength, "black");
	svgg.setXAxisLabels( xgridpoints, "Arial,san-serif", 0.2, axis_label_fontsize, axis_ticklength);
	// Arial san-serif は、alphabetic基線がいつも0.2ズレているのか？

	// 目盛ラベルの数値の桁数はどうなるのか。。

	svgg.drawYAxisTicks( ygridpoints, axis_ticklength, "black");
	svgg.setYAxisLabels( ygridpoints, "Arial,san-serif", 0.2, axis_label_fontsize, axis_ticklength);

	svgg.setGraphTitle( graph_title, "Arial,san-serif", 0.2, graph_title_fontsize, outermargin);
	// タイトル文字列内で、"<"とかを自動でエスケープしたい。

	svgg.setXAxisTitle( xaxis_title, "Arial,san-serif", 0.2,  axis_title_fontsize, outermargin);
	svgg.setYAxisTitle( yaxis_title, "Arial,san-serif", 0.2,  axis_title_fontsize, outermargin);
	
	// 周辺情報記載の終了

	// TODO: 軸の単位の記載→優先順位は低い。

	// SVGアニメをパワポに貼っても動かないらしい。

	// textタグで、IEやWordはdominant-baselineが効かないらしい。
	// （指定してもdominant-baseline="alphabetic"扱いになる。）

	// 以上、<g>での一括指定ができるところが多いが、とりあえず無視する。



	// 枠線
	svgg.drawGraphPaneFrame( "black");



	// TODO: フォントサイズを自動調節する？

	// SVGのviewBoxについて：アスペクト比が違っているとわかりにくい。
	// （強制的に余白がつくられたりするか、強制的に拡大縮小して円が歪んだりする）ので、
	// svgタグのサイズとviewBoxのサイズを合わせたい。
	
	// ここを書いていく。

	
	return svgg;

}


// [min0, max0]に、いい感じの間隔で点をとる。
// k0個以上で最小の点を返す。
// newminがtrueのとき、得られた間隔に乗る新しいminも返す。
// newmaxがtrueのとき、得られた間隔に乗る新しいmaxも返す。
// 以下のサイトのアルゴリズムに近そう。
// https://imagingsolution.net/program/autocalcgraphstep/
std::vector <double>
getGridPoints( double min0, double max0, int k0 /*= 4*/, bool newmin /*= true*/, bool newmax /*= true*/)
{

	using namespace std;

	std::vector <double> ret;
	
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
	std::vector <double> headcands = { 5.0, 2.5, 2.0, 1.0};

	double interval;

	bool loop = true;
	while ( loop){

		for ( auto h : headcands){

			ret.clear();
			interval = base10val * h;

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

	if ( newmin == true){
		double oldmin = ret.front();
		if ( oldmin == min0){
			// if the first point already obtained is equal to min0
			// do nothing
		} else {
			ret.insert( ret.begin(), oldmin - interval);
		}
	}

	if ( newmax == true){
		double oldmax = ret.back();
		if ( oldmax == max0){
			// if the last point already obtained is equal to max0
			// do nothing
		} else {
			ret.push_back( oldmax + interval);
		}
	}

	return ret;

}


/* ********** Definitions of Member Functions ********** */

/* ***** class SvgFile ***** */

void SvgFile :: addRect( const SvgRect &r0)
{
	filecontent.push_back( r0.getContent());
}

void SvgFile :: addCircle( const SvgCircle &c0)
{
	filecontent.push_back( c0.getContent());
}

void SvgFile :: addEllipse( const SvgEllipse &e0)
{
	filecontent.push_back( e0.getContent());
}

void SvgFile :: addLine( const SvgLine &l0)
{
	filecontent.push_back( l0.getContent());
}

void SvgFile :: addText( const SvgText &t0)
{
	filecontent.push_back( t0.getContent());
}


/* ***** class SvgGraph ***** */

// Cambusの座標系を定める。
void 
SvgGraph :: 
setCambus( const Cambus &c0)
{
	cam = c0;
}

// SVG全体の背景色を設定
void 
SvgGraph :: 
setBackground( const std::string &b0)
{
	SvgRect r1( 0, 0, svgwidth, svgheight);
	r1.addFill( b0);
	r1.addStroke( b0);
	addRectActu( r1);
}

// graphpane = cambus の背景色を塗る。
void 
SvgGraph :: 
setGraphPaneColor( const std::string &cambuscolor)
{
	
	{
		SvgRect r( cam.actuXMin, cam.actuYMin, cam.actuWidth, cam.actuHeight);
		r.addFill( cambuscolor);
		r.addStroke( cambuscolor);
		addRectActu( r);
	}

}

// x軸の目盛を示すグリッド線を引く。
void 
SvgGraph :: 
drawXGridLines(
	const std::vector <double> &xgridpoints,
	const std::string &linecolor 
)
{

	for ( auto v : xgridpoints){

		Point theoP1( v, cam.theoYMax); // top
		Point theoP2( v, cam.theoYMin); // bottom 
		Point actuP1 = cam.getActualFromTheoretical( theoP1);
		Point actuP2 = cam.getActualFromTheoretical( theoP2);

		SvgLine li( actuP1.x, actuP1.y, actuP2.x, actuP2.y);
		li.addStroke( linecolor);
		li.addStrokewidth( 1);
		addLineActu( li);;

	}

}

// y軸の目盛を示すグリッド線を引く。
void 
SvgGraph :: 
drawYGridLines(
	const std::vector <double> &ygridpoints,
	const std::string &linecolor 
)
{

	for ( auto v : ygridpoints){

		Point theoP1( cam.theoXMin, v); // left
		Point theoP2( cam.theoXMax, v); // right 
		Point actuP1 = cam.getActualFromTheoretical( theoP1);
		Point actuP2 = cam.getActualFromTheoretical( theoP2);

		SvgLine li( actuP1.x, actuP1.y, actuP2.x, actuP2.y);
		li.addStroke( linecolor);
		li.addStrokewidth( 1);
		addLineActu( li);;

	}

}

// 度数を示すバーを描く。
// 注：これを目盛グリッド線よりもあとに描くべし。グリッド線を「上書き」してほしいから。
void 
SvgGraph :: 
drawBins(
	const std::vector <double> &leftvec, 
	const std::vector <double> &rightvec, 
	const std::vector <int>    &counts, 
	const std::string          &color,
	bool animated /* = false */
)
{

	for ( int i = 0; i < counts.size(); i++){

		Point theoP1( leftvec[ i], counts[ i]); // left-top
		Point theoP2( rightvec[ i], 0); // right-bottom 
		Point actuP1 = cam.getActualFromTheoretical( theoP1);
		Point actuP2 = cam.getActualFromTheoretical( theoP2);

		if ( animated == true ){

			double rect_height = actuP2.y - actuP1.y;
			SvgRect rect( actuP1.x, actuP1.y, actuP2.x - actuP1.x, rect_height); 
			rect.addFill( color);
			rect.addStroke( color);

			rect.addAnimate( "height", 0, rect_height, 1);
			rect.addAnimate( "y", actuP2.y, actuP1.y, 1);

			svgf.addRect( rect);

		} else {

			SvgRect rect( actuP1.x, actuP1.y,actuP2.x - actuP1.x, actuP2.y - actuP1.y); 
			rect.addFill( color);
			rect.addStroke( color);
			svgf.addRect( rect);

		}

	}

}

// 散布図となる点を描く。
// 注：これを目盛グリッド線よりもあとに描くべし。グリッド線を「上書き」してほしいから。
void 
SvgGraph :: 
drawPoints(
	const std::vector <double> &xvec, 
	const std::vector <double> &yvec, 
	const std::string          &color
)
{

	int n = xvec.size();
	if ( xvec.size() < yvec.size()){
		n = yvec.size();
	}

	for ( int i = 0; i < n; i++){

		Point theoP( xvec[ i], yvec[ i]); 
		Point actuP = cam.getActualFromTheoretical( theoP);

		SvgCircle ci( actuP.x, actuP.y, 1);
		ci.addFill( color);
		ci.addStroke( color);
		svgf.addCircle( ci);

	}

}

// 円を描く。
// 注：これを目盛グリッド線よりもあとに描くべし。グリッド線を「上書き」してほしいから。
void 
SvgGraph :: 
drawCircle(
	double cx, double cy, double r,
	const std::string &color
)
{

	// 縦方向と横方向の縮尺が違う場合に備えて、楕円を利用する。

	Point theoP1( cx, cy); 
	Point theoP2( cx+r, cy); 
	Point theoP3( cx, cy+r); 
	Point actuP1 = cam.getActualFromTheoretical( theoP1);
	Point actuP2 = cam.getActualFromTheoretical( theoP2);
	Point actuP3 = cam.getActualFromTheoretical( theoP3);

	SvgEllipse el( actuP1.x, actuP1.y, 
	               actuP2.x - actuP1.x, // 横方向の径
				   actuP1.y - actuP3.y  // 縦方向の径
	);
	el.addFill( "none");
	el.addStroke( color);
	svgf.addEllipse( el);

}

// x軸の目盛のヒゲを描く。
void 
SvgGraph :: 
drawXAxisTicks(
	const std::vector <double> &xgridpoints,
	double ticklength,
	const std::string &color
)
{

	for ( auto v : xgridpoints){

		double actuX = cam.getXActualFromTheoretical( v);

		double x1 = actuX;
		double y1 = cam.actuYMax;
		double x2 = actuX;
		double y2 = cam.actuYMax + ticklength; 

		SvgLine li( x1, y1, x2, y2);
		li.addStroke( color); 
		li.addStrokewidth( 1); 
		svgf.addLine( li);		

	}

}

// y軸の目盛のヒゲを描く。
void 
SvgGraph :: 
drawYAxisTicks(
	const std::vector <double> &ygridpoints,
	double ticklength,
	const std::string &color
)
{

	for ( auto v : ygridpoints){

		double actuY = cam.getYActualFromTheoretical( v);

		double x1 = cam.actuXMin;
		double y1 = actuY;
		double x2 = cam.actuXMin - ticklength; 
		double y2 = actuY; 

		SvgLine li( x1, y1, x2, y2);
		li.addStroke( color); 
		li.addStrokewidth( 1); 
		svgf.addLine( li);		

	}

}

// x軸の目盛のラベルを描く。
void 
SvgGraph :: 
setXAxisLabels(
	const std::vector <double> &xgridpoints, 
	const std::string &fontface,
	double fontbase, // alphabetic基線の下端からのズレ（割合）
	double fontsize,
	double ticklength       
)
{

	for ( auto v : xgridpoints){

		double actuX = cam.getXActualFromTheoretical( v);

		// textタグで、IEやWordはdominant-baselineが効かないらしい。
		// （指定してもdominant-baseline="alphabetic"扱いになる。）
		
		// 描画領域の下端から以下の余白だけ離す。
		// alphabeticの基線は指定座標よりfontbaseだけ上なので、その分をずらしている。
		double actuY = cam.actuYMax + ticklength + fontsize * ( 1.0 - fontbase);

		// vの桁数はどうなるのか。。 

		SvgText te( actuX, actuY, v); // vはstringに変換される。
		te.addFontfamily( fontface);
		te.addFontsize( fontsize);
		te.addTextanchor( "middle"); // 左右方向に中央揃えをする。
		te.addDominantbaseline( "alphabetic"); // これしかIEやWordが対応していない。
		svgf.addText( te);		

	}

}

// y軸の目盛のラベルを描く。
void 
SvgGraph ::
setYAxisLabels(
	const std::vector <double> &ygridpoints, 
	const std::string &fontface,
	double fontbase, // alphabetic基線の下端からのズレ（割合）
	double fontsize,
	double ticklength       
)
{

	/*
	文字列を回転させる方法を探った→svgtest04.svgとsvgtest05.svg
	　svgtest04.svgで2つの方法を試したが、もっとシンプルにしたかった。
	　svgtest05.svgで、transform属性を使えばよいことがわかった。
	*/
	for ( auto v : ygridpoints){

		double actuY = cam.getYActualFromTheoretical( v);

		// alphabetic基線に合わせるためにfontbaseだけずらしている。
		double actuX = cam.actuXMin - ticklength - fontsize * fontbase;
		
		SvgText te( actuX, actuY, v); // vはstringに変換される。
		te.addFontfamily( fontface);
		te.addFontsize( fontsize);
		te.addTextanchor( "middle"); 
		te.addDominantbaseline( "alphabetic"); 
		te.addRotate( 270, actuX, actuY); // 回転の中心が各点で異なるので、一括指定できない。
		svgf.addText( te);				

	}

}

// グラフタイトルを表示
void 
SvgGraph :: 
setGraphTitle(
	const std::string &title,
	const std::string &fontface,
	double fontbase, // alphabetic基線の下端からのズレ（割合）
	double fontsize,
	double outermargin 
)
{

	double actuX = std::round( svgwidth / 2);
	double actuY = outermargin + fontsize * ( 1 - fontbase);

	SvgText te( actuX, actuY, title);
	te.addFontfamily( fontface);
	te.addFontsize( fontsize);
	te.addTextanchor( "middle"); // 左右方向に中央揃えをする。
	te.addDominantbaseline( "alphabetic"); // これしかIEやWordが対応していない。
	svgf.addText( te);	

}

// x軸タイトルを表示
void 
SvgGraph :: 
setXAxisTitle(
	const std::string &title,
	const std::string &fontface,
	double fontbase, // alphabetic基線の下端からのズレ（割合）
	double fontsize,
	double outermargin 
)
{

	double actuX = std::round( cam.getActualMidX());
	double actuY = svgheight - outermargin - fontsize * fontbase; 

	SvgText te( actuX, actuY, title);
	te.addFontfamily( fontface);
	te.addFontsize( fontsize);
	te.addTextanchor( "middle"); 
	te.addDominantbaseline( "alphabetic"); 
	svgf.addText( te);	

}

// y軸タイトルを表示
void 
SvgGraph ::
setYAxisTitle(
	const std::string &title,
	const std::string &fontface,
	double fontbase, // alphabetic基線の下端からのズレ（割合）
	double fontsize,
	double outermargin 
)
{

	double actuX = outermargin + fontsize * ( 1 - fontbase);
	double actuY = std::round( cam.getActualMidY());

	SvgText te( actuX, actuY, title);
	te.addFontfamily( fontface);
	te.addFontsize( fontsize);
	te.addTextanchor( "middle"); 
	te.addDominantbaseline( "alphabetic"); 
	te.addRotate( 270, actuX, actuY); // 回転の中心が各点で異なるので、一括指定できない。
	svgf.addText( te);	

}

// graphpaneの枠線を描画
// 注：この描画は最後にすべき。
// 　　fillは透過させる。
void 
SvgGraph :: 
drawGraphPaneFrame(
	const std::string color
)
{
	SvgRect r( cam.actuXMin, cam.actuYMin, cam.actuWidth, cam.actuHeight);
	r.addFillopacity( 0); // 完全透過
	r.addStroke( color);
	r.addStrokewidth( 1);
	svgf.addRect( r);
}

// 座標変換せずに描画
void 
SvgGraph :: 
addRectActu( const SvgRect &r0)
{
	svgf.addRect( r0);
}

// 座標変換せずに描画
void 
SvgGraph :: 
addLineActu( const SvgLine &l0)
{
	svgf.addLine( l0);
}

// ファイルに書き出すメソッド
// 正常終了でtrue、異常終了でfalseを返す。
bool 
SvgGraph :: 
writeFile( const std::string &fn0)
{
	return svgf.writeFile( fn0);
}