
/*
	
	ksvggraph00.cpp
	Ver. k09.00
	
	Written by Koji Yamamoto
	Copyright (C) 2020 Koji Yamamoto

	SvgGraphの余白などのとりかたの説明を書く。

	記事を書く？
	・「いい感じの間隔」で点をとることについて？
	・SVGの謎について？ksvg.cppに書いた。

	TODO:
	アニメーション実装でコメントアウトしているところを追加したい。
	軸の単位の記載→優先順位は低い。
	フォントサイズを自動調節

	TODO:
	SvgHistogramMakerのコンストラクタで、
	width,bin,startなどを指定できるようにしたい。

*/	


/* ********** Preprocessor Directives ********** */

#ifndef ksvggraph_cpp_include_guard
#define ksvggraph_cpp_include_guard

#include <k09/ksvg00.cpp>
#include <k09/kstat02.cpp>
#include <map>


/* ********** Namespace Declarations/Directives ********** */


/* ********** Class Declarations ********** */

class Coordinates;

class SvgGraph;
class GraphAddable;
class GraphBasicShape;
class GraphRect;
class GraphLine;
class GraphDot; 
class GraphEllipse; 

class GraphRectAnimator;
class BuildupGraphRectAnimator;

class SvgGraphMakerBase;
class SvgHistogramMaker;
class SvgScatterMaker;


/* ********** Enum Definitions ********** */


/* ********** Function Declarations ********** */

std::vector <double>
getGridPoints( double, double, int = 4, bool = true, bool = true);


/* ********** Class Definitions ********** */

// 座標変換のためのクラス（メンバはすべてpublic）
// theoないしtheoreticalは論理座標系を示す。
// actuないしactualは実際に描画時に使われる座標系を示す。
// 論理座標系では、y座標は大きいほど「上」の位置を示す。
// 実際の座標系では、y座標は大きいほど「下」の位置を示す。
class Coordinates {

public: 

	bool actuValid; // false if actual-coordinate is not set yet
	bool theoValid; // false if theoretical-coordinate is not set yet

	// 実際の座標系での、枠の範囲
	double actuXMin;
	double actuYMin;
	double actuXMax;
	double actuYMax; 
	double actuWidth;
	double actuHeight;

	// 論理座標系での、枠の範囲
	double theoXMin;
	double theoYMin;
	double theoXMax;
	double theoYMax; 
	double theoWidth;
	double theoHeight; 

	Coordinates( void) : actuValid( false), theoValid( false)
	{}

	void setTheoretical( double xmin0, double ymin0, double xmax0, double ymax0)
	{
		theoXMin = xmin0;
		theoYMin = ymin0;
		theoXMax = xmax0;
		theoYMax = ymax0; 
		theoWidth = xmax0 - xmin0;
		theoHeight = ymax0 - ymin0; 
		theoValid = true;
	}

	void setActual( double xmin0, double ymin0, double xmax0, double ymax0)
	{
		actuXMin = xmin0;
		actuYMin = ymin0;
		actuXMax = xmax0;
		actuYMax = ymax0; 
		actuWidth = xmax0 - xmin0;
		actuHeight = ymax0 - ymin0;
		actuValid = true; 
	}

	// x座標のみを算出→論理座標系表現から実際の座標系表現を作成。
	double getXActualFromTheoretical( double x0) const
	{

		if ( theoValid == false || actuValid == false){
			return std::numeric_limits<double>::quiet_NaN();
		}

		double retx = ( x0 - theoXMin) / theoWidth  * actuWidth  + actuXMin; 
		return retx;

	}

	// y座標のみを算出→論理座標系表現から実際の座標系表現を作成。
	double getYActualFromTheoretical( double y0) const
	{
		
		if ( theoValid == false || actuValid == false){
			return std::numeric_limits<double>::quiet_NaN();
		}

		double rety = ( theoYMax - y0) / theoHeight * actuHeight + actuYMin; 
		return rety;

	}

	// 実際の座標系での中点のxを返す。
	double getActualMidX( void) const
	{

		if ( actuValid == false){
			return std::numeric_limits<double>::quiet_NaN();
		}

		return ( actuXMin + actuWidth / 2);

	}

	// 実際の座標系での中点のyを返す。
	double getActualMidY( void) const
	{

		if ( actuValid == false){
			return std::numeric_limits<double>::quiet_NaN();
		}

		return ( actuYMin + actuHeight / 2);

	}

};


class SvgGraph { 

protected:

	SvgFile svgf;

	Coordinates coord;
	double svgwidth;  // width of the whole SVG
	double svgheight; // height of the whole SVG

	double outermargin;
	double graph_title_fontsize;
	double graph_title_margin; // グラフタイトルの下のマージン
	double axis_title_fontsize; 
	double axis_title_margin; // 軸タイトルと軸ラベルの間のマージン
	double axis_label_fontsize; 
	double axis_ticklength;

	std::string graph_title;
	std::string xaxis_title;
	std::string yaxis_title;

	std::vector <double> xgridpoints;
	std::vector <double> ygridpoints;

public:

	SvgGraph( void) = delete; 

	SvgGraph(
		double w0, double h0, double x1, double y1, double x2, double y2
	);

	~SvgGraph( void);

	void setDefault( void);
	void setDefaultCoordAndGridpoints( double, double, double, double);

	void setCoordinates( const Coordinates &);
	void setXGridPoints( const std::vector <double> &);
	void setYGridPoints( const std::vector <double> &);

	void setGraphTitle( const std::string &);
	void setXAxisTitle( const std::string &);
	void setYAxisTitle( const std::string &);
	void prepareGraph( void);

	void addElement( const GraphAddable &);
	
	void addRectActu( const SvgRect &); 
	void addLineActu( const SvgLine &); 
	void addTextActu( const SvgText &);

	void addBackground( const std::string &);

	void addGraphPaneColor( const std::string &);

	void startDrawingGraphPane( void);
	void endDrawingGraphPane( void);

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

	void drawEllipse(
		double,
		double, 
		double,
		double,
		const std::string &
	);

	void addXAxisTicks( 
		const std::vector <double> &, double, const std::string &
	);

	void addYAxisTicks( 
		const std::vector <double> &, double, const std::string &
	);

	void addXAxisLabels(
		const std::vector <double> &, const std::string &,
		double, double, double
	);
	
	void addYAxisLabels(
		const std::vector <double> &, const std::string &,
		double, double, double
	);
	
	void addGraphTitle(
		const std::string &, const std::string &, double, double, double
	);

	void addXAxisTitle(
		const std::string &, const std::string &, double, double, double
	);

	void addYAxisTitle( 
		const std::string &, const std::string &, double, double, double
	);

	void addGraphPaneFrame( const std::string);

	bool writeFile( const std::string &);


	// friend classes
	friend class SvgHistogramMaker; 

}; 


// interface for addable SvgGraph elements 
class GraphAddable {
public:
	virtual std::string getContent( const Coordinates &) const = 0;
};


// Base class for GraphRect, etc.
class GraphBasicShape {

protected:

	std::string fill;
	std::string stroke;
	std::string strokewidth;
	std::string fillopacity;

	std::string getBasicShapeAttr( void) const;

public:

	GraphBasicShape( void);
	virtual ~GraphBasicShape( void);

	virtual void setFill( const std::string &s0);
	virtual void setStroke( const std::string &s0);
	virtual void setStrokewidth( double v0);
	virtual void setFillopacity( double v0);

};


// 論理座標系で指定する。(x,y)は「左下」の点。
class GraphRect : public GraphBasicShape, public GraphAddable {

private:

	double x;
	double y;
	double w;
	double h;

	std::vector < std::unique_ptr <GraphRectAnimator> > animatorpvec;

public:

	GraphRect( double x0, double y0, double w0, double h0);
	~GraphRect( void);

	double getX( void) const;
	double getY( void) const;
	double getW( void) const;
	double getH( void) const;

	std::string getContent( const Coordinates &coord) const;

	void addAnimateBuildup( double);

};


// 論理座標系で指定する。
class GraphLine : public GraphBasicShape, public GraphAddable {

private:

	double x1;
	double y1;
	double x2;
	double y2;

// ↓あとで
//	std::vector < std::unique_ptr <GraphLineAnimator> > animatorpvec;

public:

	GraphLine( double xa, double ya, double xb, double yb);
	~GraphLine( void);

// ↓アニメーションを入れるときにつくる。
//	double getX1( void) const;
//	double getY1( void) const;
//	double getX2( void) const;
//	double getY2( void) const;

	std::string getContent( const Coordinates &coord) const;

//	void addAnimateOpacity( double); // 透過度をゼロからMAXに？

};


// 論理座標系で指定する。
// ただしドット専用。
class GraphDot : public GraphBasicShape, public GraphAddable {

private:

	double cx;
	double cy;
	double size; // これだけは実際の座標系での大きさ（半径）

// ↓あとで
//	std::vector < std::unique_ptr <GraphCircleAnimator> > animatorpvec;

public:

	GraphDot( double cx0, double cy0, double size0);
	~GraphDot( void);

// ↓アニメーションを入れるときにつくる。
//	double getCx( void) const;
//	double getCy( void) const;
//	double getR( void) const;

	std::string getContent( const Coordinates &coord) const;

//	void addAnimateDelay( double); // ある秒数から出現？

};


// 論理座標系で指定する。
class GraphEllipse : public GraphBasicShape, public GraphAddable {

private:

	double cx;
	double cy;
	double rx; // x軸方向の半径
	double ry; // y軸方向の半径

public:

	GraphEllipse( double cx0, double cy0, double rx0, double ry0);
	~GraphEllipse( void);

	std::string getContent( const Coordinates &coord) const;

};


// interface for animators of SvgGraph elements 
class GraphRectAnimator {
public:
	virtual std::string getContent(
		const GraphRect &, const Coordinates &
	) const = 0;
};


// 長方形が下から伸びてくるアニメーション
class BuildupGraphRectAnimator : public GraphRectAnimator {

private:

	double sec;

public:

	BuildupGraphRectAnimator( void) = delete;
	BuildupGraphRectAnimator( double);
	~BuildupGraphRectAnimator( void);
	
	std::string getContent( const GraphRect &, const Coordinates &) const;

};


class SvgGraphMakerBase {

protected:

	std::string graph_title;
	std::string xaxis_title; 
	std::string yaxis_title; 

	double svgwidth;
	double svgheight;
	double outermargin;
	double graph_title_fontsize;
	double graph_title_margin; // グラフタイトルの下のマージン
	double axis_title_fontsize;
	double axis_title_margin; // 軸タイトルと軸ラベルの間のマージン
	double axis_label_fontsize;
	double axis_ticklength;

public:

	SvgGraphMakerBase( void);

	virtual ~SvgGraphMakerBase( void);

	void setDefaults( void);
	void modifyMetrics( const std::map <string, double> &);
	
	void setGraphTitle( const std::string &s);	
	void setXAxisTitle( const std::string &s);	
	void setYAxisTitle( const std::string &s);

};


class SvgHistogramMaker : public SvgGraphMakerBase {

private:

	std::vector <double> leftvec; 
	std::vector <double> rightvec; 
	std::vector <int> counts; 
	bool animated; 

	// Below are inherited from SvgGraphMakerBase
	// ----------
	// std::string graph_title;
	// std::string xaxis_title; 
	// std::string yaxis_title; 
	// double svgwidth;
	// double svgheight;
	// double outermargin;
	// double graph_title_fontsize;
	// double graph_title_margin; // グラフタイトルの下のマージン
	// double axis_title_fontsize;
	// double axis_title_margin; // 軸タイトルと軸ラベルの間のマージン
	// double axis_label_fontsize;
	// double axis_ticklength;

public:

	SvgHistogramMaker( void) = delete;

	SvgHistogramMaker( 
		const std::vector <double> &lv0,
		const std::vector <double> &rv0,
		const std::vector <int> &c0,
		bool an0 = false
	);

	SvgHistogramMaker(
		const std::vector <double> &data,
		bool an0 = false
	);

	~SvgHistogramMaker( void);

	SvgGraph createGraph( void);

	// Below are inherited from SvgGraphMakerBase
	// ----------
	// void setDefaults( void);
	// void setGraphTitle( const std::string &s);	
	// void setXAxisTitle( const std::string &s);	
	// void setYAxisTitle( const std::string &s);

};


class SvgScatterMaker : public SvgGraphMakerBase {

private:

	std::vector <double> xvec; 
	std::vector <double> yvec; 

	// Below are inherited from SvgGraphMakerBase
	// ----------
	// std::string graph_title;
	// std::string xaxis_title; 
	// std::string yaxis_title; 
	// double svgwidth;
	// double svgheight;
	// double outermargin;
	// double graph_title_fontsize;
	// double graph_title_margin; // グラフタイトルの下のマージン
	// double axis_title_fontsize;
	// double axis_title_margin; // 軸タイトルと軸ラベルの間のマージン
	// double axis_label_fontsize;
	// double axis_ticklength;

public:

	SvgScatterMaker( void) = delete;

	SvgScatterMaker( 
		const std::vector <double> &x0,
		const std::vector <double> &y0
	);

	~SvgScatterMaker( void);

	SvgGraph createGraph( void);

	// Below are inherited from SvgGraphMakerBase
	// ----------
	// void setDefaults( void);
	// void setGraphTitle( const std::string &s);	
	// void setXAxisTitle( const std::string &s);	
	// void setYAxisTitle( const std::string &s);

};


/* ********** Global Variables ********** */


/* ********** Definitions of Static Member Variables ********** */


/* ********** Function Definitions ********** */

// [min0, max0]に、「いい感じの間隔」で点をとる。
// k0個以上で最小個数の点を返す。
// この個数には、以下の新しいminと新しいmaxは含まない。
// 
// newminがtrueのとき、得られた間隔に乗る新しいminも返す。
// newmaxがtrueのとき、得られた間隔に乗る新しいmaxも返す。
// 以下のサイトのアルゴリズムに近そう。
// https://imagingsolution.net/program/autocalcgraphstep/
std::vector <double>
getGridPoints(
	double min0,
	double max0,
	int k0,      // = 4
	bool newmin, // = true
	bool newmax  // = true
)
{
	
	using namespace std;
	
	std::vector <double> ret;
	
	// この数以上の最小の点を返すようにする。
	int minnpoints = k0; 

	// error
	if ( min0 >= max0){ 
		alert( "getGridPoints()");
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
			
			// setting startpoint;
			// to avoid startpoint being "-0", we do a little trick.
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


/* ***** class SvgGraph ***** */

SvgGraph :: 
SvgGraph(
	double w0, double h0, double x1, double y1, double x2, double y2
)
: svgf( w0, h0, x1, y1, x2, y2), coord(), svgwidth( w0), svgheight( h0),
  xgridpoints(), ygridpoints()
{
	setDefault();
}

SvgGraph :: 
~SvgGraph( void)
{}

void
SvgGraph :: 
setDefault( void)
{

	svgwidth = 500;
	svgheight = 500;
	outermargin = 20;
	graph_title_fontsize = 20;
	graph_title_margin = 10; // グラフタイトルの下のマージン
	axis_title_fontsize = 20;
	axis_title_margin = 5; // 軸タイトルと軸ラベルの間のマージン
	axis_label_fontsize = 14;
	axis_ticklength = 5;

	graph_title = "(No Title)";
	xaxis_title = "x (Not Labeled)";
	yaxis_title = "y (Not Labeled)";

}

// デフォルトの方法で、CoordinatesとX軸・Y軸の目盛の位置を決める。
// 論理座標系での、x・yの最小値・最大値を与える。
void
SvgGraph :: 
setDefaultCoordAndGridpoints(
	double xmin, double xmax, double ymin, double ymax
)
{
	
	// グラフ描画領域の座標（左上、右下）
	double graphpane_actuX1 = 
		outermargin + axis_title_fontsize + axis_title_margin + 
		axis_label_fontsize + axis_ticklength; 
	double graphpane_actuY1 = 
		outermargin + graph_title_fontsize + graph_title_margin;
	double graphpane_actuX2 = 
		svgwidth - outermargin; 
	double graphpane_actuY2 = 
		svgheight - 
		( outermargin + axis_title_fontsize + axis_title_margin +
		  axis_label_fontsize + axis_ticklength);
	
	double graphwidth = graphpane_actuX2 - graphpane_actuX1;
	double graphheight = graphpane_actuY2 - graphpane_actuY1;

	// ちょうどいい間隔のグリッド線の点を得る。
	// x軸
	xgridpoints = getGridPoints( xmin, xmax);
	// y軸
	ygridpoints = getGridPoints( ymin, ymax);
	
	// 描画範囲は、Gridpointsのさらに5%外側にする。
	double theoWidthTemp = xgridpoints.back() - xgridpoints.front();
	double theoXMin = xgridpoints.front() - 0.05 * theoWidthTemp;
	double theoXMax = xgridpoints.back() + 0.05 * theoWidthTemp;
	
	double theoHeightTemp = ygridpoints.back() - ygridpoints.front();
	double theoYMin = ygridpoints.front() - 0.05 * theoHeightTemp;
	double theoYMax = ygridpoints.back() + 0.05 * theoHeightTemp;
	
	// Coordinatesの設定
	coord.setActual(
		graphpane_actuX1, graphpane_actuY1, graphpane_actuX2, graphpane_actuY2
	);
	coord.setTheoretical( theoXMin, theoYMin, theoXMax, theoYMax);
	
}

// GraphPaneの座標系を示すCoordinatesを設定する。
void 
SvgGraph :: 
setCoordinates( const Coordinates &c0)
{
	coord = c0;
}

void
SvgGraph :: 
setXGridPoints( const std::vector <double> &vec0)
{
	xgridpoints = vec0;
}

void
SvgGraph :: 
setYGridPoints( const std::vector <double> &vec0)
{
	ygridpoints = vec0;
}

void
SvgGraph :: 
setGraphTitle( const std::string &s0)
{
	graph_title = s0;
}

void
SvgGraph :: 
setXAxisTitle( const std::string &s0)
{
	xaxis_title = s0;
}

void
SvgGraph :: 
setYAxisTitle( const std::string &s0)
{
	yaxis_title = s0;
}

void
SvgGraph ::  
prepareGraph( void)
{

	// 背景の描画開始
	addBackground( "whitesmoke");
	addGraphPaneColor( "gainsboro");
	// 背景の描画終了

	// 周辺情報記載の開始
	addXAxisTicks( xgridpoints, axis_ticklength, "black");
	addXAxisLabels(
		xgridpoints, "Arial,san-serif",
		0.2, axis_label_fontsize, axis_ticklength
	);
	// Arial san-serif は、alphabetic基線がいつも0.2ズレているのか？
	// 目盛ラベルの数値の桁数はどうなるのか。。
	
	addYAxisTicks( ygridpoints, axis_ticklength, "black");
	addYAxisLabels(
		ygridpoints, "Arial,san-serif",
		0.2, axis_label_fontsize, axis_ticklength
	);
	
	addGraphTitle(
		graph_title, "Arial,san-serif",
		0.2, graph_title_fontsize, outermargin
	);
	// タイトル文字列内で、"<"とかを自動でエスケープしたい。
	
	addXAxisTitle(
		xaxis_title, "Arial,san-serif",
		0.2,  axis_title_fontsize, outermargin
	);
	addYAxisTitle(
		yaxis_title, "Arial,san-serif",
		0.2,  axis_title_fontsize, outermargin
	);
	drawXGridLines( xgridpoints, "silver");
	drawYGridLines( ygridpoints, "silver");
	// 周辺情報記載の終了

	// 枠線
	addGraphPaneFrame( "black");

}

// 要素オブジェクトを追加。
// elem内でcamを使って座標変換することになる。
void 
SvgGraph :: 
addElement( const GraphAddable &elem)
{
	svgf.addFileContent( elem.getContent( coord));
}

// 座標変換せずに描画
void 
SvgGraph :: 
addRectActu( const SvgRect &r0)
{
	svgf.addElement( r0);
}

// 座標変換せずに描画
void 
SvgGraph :: 
addLineActu( const SvgLine &l0)
{
	svgf.addElement( l0);
}

// 座標変換せずにテキストを追加
void 
SvgGraph :: 
addTextActu( const SvgText &t0)
{
	svgf.addElement( t0);
}

// SVG全体の背景色を設定
void 
SvgGraph :: 
addBackground( const std::string &b0)
{
	SvgRect r1( 0, 0, svgwidth, svgheight);
	r1.addFill( b0);
	r1.addStroke( b0);
	addRectActu( r1);
}

// graphpane = cambus の背景色を塗る。
void 
SvgGraph :: 
addGraphPaneColor( const std::string &cambuscolor)
{
	
	{
		SvgRect r(
			coord.actuXMin, coord.actuYMin, coord.actuWidth, coord.actuHeight
		);
		r.addFill( cambuscolor);
		r.addStroke( cambuscolor);
		addRectActu( r);
	}

}

// GraphPaneでの描画を開始する。
// GraphPaneでの描画をはみ出さないようにする。
// 座標系は変化しない。
// 必ずendDrawingGraphPane()を終了時に実行すべし。
void 
SvgGraph :: 
startDrawingGraphPane( void)
{
	double actuX = coord.actuXMin;
	double actuY = coord.actuYMin;
	double paneWidth = coord.actuWidth;
	double paneHeight = coord.actuHeight;

	std::stringstream ss;
	ss << "<svg "
	   << "x=\"" << actuX << "\"" << " "
	   << "y=\"" << actuY << "\"" << " " 
	   << "width=\"" << paneWidth << "\"" << " " 
	   << "height=\"" << paneHeight << "\"" << " " 
	   << "viewBox=\""
	       << actuX << " " << actuY << " " 
		   << paneWidth << " " << paneHeight
	   << "\"" << ">";
	svgf.addFileContent( ss.str());

}

// GraphPaneでの描画を終了する。
void 
SvgGraph :: 
endDrawingGraphPane( void)
{
	svgf.addFileContent( "</svg>");
}

// x軸の目盛を示すグリッド線を引く。
void 
SvgGraph :: 
drawXGridLines(
	const std::vector <double> &xgridpoints,
	const std::string &linecolor 
)
{

	double theoy_top = coord.theoYMax;
	double theoy_bottom = coord.theoYMin;

	for ( auto v : xgridpoints){

		GraphLine li( v, theoy_top, v, theoy_bottom);
		li.setStroke( linecolor);
		li.setStrokewidth( 1);
		addElement( li); 

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

	double theox_left = coord.theoXMin;
	double theox_right = coord.theoXMax;

	for ( auto v : ygridpoints){

		GraphLine li( theox_left, v, theox_right, v);
		li.setStroke( linecolor);
		li.setStrokewidth( 1);
		addElement( li); 

	}

}

// 度数を示すバーを描く。
// animted = trueのとき、下から1秒間で伸びてくるアニメーションが加わる。
// 注：これを目盛グリッド線よりもあとに描くべし。
// 　　グリッド線を「上書き」してほしいから。
void 
SvgGraph :: 
drawBins(
	const std::vector <double> &leftvec, 
	const std::vector <double> &rightvec, 
	const std::vector <int>    &counts, 
	const std::string          &color,
	bool animated // = false 
)
{

	for ( int i = 0; i < counts.size(); i++){

		GraphRect rect( leftvec[ i], 0, rightvec[ i] - leftvec[ i], counts[ i]);
		rect.setFill( color);
		rect.setStroke( color);

		if ( animated == true){
			rect.addAnimateBuildup( 1);
		}

		addElement( rect); 

	}

}

// 散布図となる点を描く。
// 注：これを目盛グリッド線よりもあとに描くべし。
// 　　グリッド線を「上書き」してほしいから。
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

		GraphDot dot( xvec[ i], yvec[ i], 1);
		dot.setFill( color);
		dot.setStroke( color);
		addElement( dot);

	}

}

// 楕円を描く。
// rx==ryのとき、（論理座標では）真円になる。
// 注：これを目盛グリッド線よりもあとに描くべし。
// 　　グリッド線を「上書き」してほしいから。
void 
SvgGraph :: 
drawEllipse(
	double cx, double cy, double rx, double ry,
	const std::string &color
)
{

	GraphEllipse el( cx, cy, rx, ry);
	el.setFill( "none");
	el.setStroke( color);
	addElement( el);

}

// x軸の目盛のヒゲを描く。
void 
SvgGraph :: 
addXAxisTicks(
	const std::vector <double> &xgridpoints,
	double ticklength,
	const std::string &color
)
{

	for ( auto v : xgridpoints){

		double actuX = coord.getXActualFromTheoretical( v);

		double x1 = actuX;
		double y1 = coord.actuYMax;
		double x2 = actuX;
		double y2 = coord.actuYMax + ticklength; 

		SvgLine li( x1, y1, x2, y2);
		li.addStroke( color); 
		li.addStrokewidth( 1); 
		svgf.addElement( li);		

	}

}

// y軸の目盛のヒゲを描く。
void 
SvgGraph :: 
addYAxisTicks(
	const std::vector <double> &ygridpoints,
	double ticklength,
	const std::string &color
)
{

	for ( auto v : ygridpoints){

		double actuY = coord.getYActualFromTheoretical( v);

		double x1 = coord.actuXMin;
		double y1 = actuY;
		double x2 = coord.actuXMin - ticklength; 
		double y2 = actuY; 

		SvgLine li( x1, y1, x2, y2);
		li.addStroke( color); 
		li.addStrokewidth( 1); 
		svgf.addElement( li);		

	}

}

// x軸の目盛のラベルを描く。
void 
SvgGraph :: 
addXAxisLabels(
	const std::vector <double> &xgridpoints, 
	const std::string &fontface,
	double fontbase, // alphabetic基線の下端からのズレ（割合）
	double fontsize,
	double ticklength       
)
{

	for ( auto v : xgridpoints){

		double actuX = coord.getXActualFromTheoretical( v);

		// textタグで、IEやWordはdominant-baselineが効かないらしい。
		// （指定してもdominant-baseline="alphabetic"扱いになる。）
		
		// 描画領域の下端から以下の余白だけ離す。
		// alphabeticの基線は指定座標よりfontbaseだけ上なので、
		// その分をずらしている。
		double actuY = 
			coord.actuYMax + ticklength + fontsize * ( 1.0 - fontbase);
		
		// vの桁数はどうなるのか。。 
		
		SvgText te( actuX, actuY, v); // vはstringに変換される。
		te.addFontfamily( fontface);
		te.addFontsize( fontsize);
		te.addTextanchor( "middle"); // 左右方向に中央揃えをする。
		te.addDominantbaseline( "alphabetic");
		// ↑これしかIEやWordが対応していない。
		
		svgf.addElement( te);		

	}

}

// y軸の目盛のラベルを描く。
void 
SvgGraph ::
addYAxisLabels(
	const std::vector <double> &ygridpoints, 
	const std::string &fontface,
	double fontbase, // alphabetic基線の下端からのズレ（割合）
	double fontsize,
	double ticklength       
)
{

	// 文字列を回転させる方法を探った→svgtest04.svgとsvgtest05.svg
	// 　svgtest04.svgで2つの方法を試したが、もっとシンプルにしたかった。
	// 　svgtest05.svgで、transform属性を使えばよいことがわかった。
	for ( auto v : ygridpoints){

		double actuY = coord.getYActualFromTheoretical( v);

		// alphabetic基線に合わせるためにfontbaseだけずらしている。
		double actuX = coord.actuXMin - ticklength - fontsize * fontbase;
		
		SvgText te( actuX, actuY, v); // vはstringに変換される。
		te.addFontfamily( fontface);
		te.addFontsize( fontsize);
		te.addTextanchor( "middle"); 
		te.addDominantbaseline( "alphabetic"); 
		te.addRotate( 270, actuX, actuY);
		// ↑回転の中心が各点で異なるので、一括指定できない。
		
		svgf.addElement( te);				

	}

}

// グラフタイトルを表示
void 
SvgGraph :: 
addGraphTitle(
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
	te.addDominantbaseline( "alphabetic"); 
	// ↑これしかIEやWordが対応していない。
	
	svgf.addElement( te);	

}

// x軸タイトルを表示
void 
SvgGraph :: 
addXAxisTitle(
	const std::string &title,
	const std::string &fontface,
	double fontbase, // alphabetic基線の下端からのズレ（割合）
	double fontsize,
	double outermargin 
)
{

	double actuX = std::round( coord.getActualMidX());
	double actuY = svgheight - outermargin - fontsize * fontbase; 

	SvgText te( actuX, actuY, title);
	te.addFontfamily( fontface);
	te.addFontsize( fontsize);
	te.addTextanchor( "middle"); 
	te.addDominantbaseline( "alphabetic"); 
	svgf.addElement( te);	

}

// y軸タイトルを表示
void 
SvgGraph ::
addYAxisTitle(
	const std::string &title,
	const std::string &fontface,
	double fontbase, // alphabetic基線の下端からのズレ（割合）
	double fontsize,
	double outermargin 
)
{

	double actuX = outermargin + fontsize * ( 1 - fontbase);
	double actuY = std::round( coord.getActualMidY());

	SvgText te( actuX, actuY, title);
	te.addFontfamily( fontface);
	te.addFontsize( fontsize);
	te.addTextanchor( "middle"); 
	te.addDominantbaseline( "alphabetic"); 
	te.addRotate( 270, actuX, actuY);
	// ↑回転の中心が各点で異なるので、一括指定できない。
	
	svgf.addElement( te);	

}

// graphpaneの枠線を描画
// 注：この描画は最後にすべき。
// 　　fillは透過させる。
void 
SvgGraph :: 
addGraphPaneFrame(
	const std::string color
)
{
	SvgRect r(
		coord.actuXMin, coord.actuYMin, coord.actuWidth, coord.actuHeight
	);
	r.addFillopacity( 0); // 完全透過
	r.addStroke( color);
	r.addStrokewidth( 1);
	svgf.addElement( r);
}

// ファイルに書き出すメソッド
// 正常終了でtrue、異常終了でfalseを返す。
bool 
SvgGraph :: 
writeFile( const std::string &fn0)
{
	return svgf.writeFile( fn0);
}


/* ***** class GraphBasicShape ***** */

std::string 
GraphBasicShape :: 
getBasicShapeAttr( void) const
{

	std::stringstream ss;

	if ( fill.size() > 0){
		ss << "fill=" << "\"" << fill << "\"" << " ";
	}
	
	if ( stroke.size() > 0){
		ss << "stroke=" << "\"" << stroke << "\"" << " ";
	}

	if ( strokewidth.size() > 0){
		ss << "stroke-width=" << "\"" << strokewidth << "\"" << " ";
	}

	if ( fillopacity.size() > 0){
		ss << "fill-opacity=" << "\"" << fillopacity << "\"" << " ";
	}

	return ss.str();

}

GraphBasicShape :: 
GraphBasicShape( void)
: fill(), stroke(), strokewidth(), fillopacity()
{}

//virtual 
GraphBasicShape :: 
~GraphBasicShape( void)
{}

//virtual 
void 
GraphBasicShape :: 
setFill( const std::string &s0)
{
	fill = s0;
}

//virtual 
void 
GraphBasicShape :: 
setStroke( const std::string &s0)
{
	stroke = s0;
}

//virtual 
void 
GraphBasicShape :: 
setStrokewidth( double v0)
{
	std::stringstream ss;
	ss << v0;
	strokewidth = ss.str();
}

//virtual 
void 
GraphBasicShape :: 
setFillopacity( double v0)
{
	std::stringstream ss;
	ss << v0;
	fillopacity = ss.str();
}


/* ***** class GraphRect ***** */

GraphRect :: 
GraphRect( double x0, double y0, double w0, double h0)
: GraphBasicShape(), x( x0), y( y0), w( w0), h( h0), animatorpvec()
{}

GraphRect :: 
~GraphRect( void)
{}

double 
GraphRect :: 
getX( void)
const
{
	return x;
}

double 
GraphRect :: 
getY( void)
const
{
	return y;
}

double 
GraphRect :: 
getW( void)
const
{
	return w;
}

double 
GraphRect :: 
getH( void)
const
{
	return h;
}

std::string 
GraphRect :: 
getContent( const Coordinates &coord) 
const
{

	double leftx   = coord.getXActualFromTheoretical( x);
	double bottomy = coord.getYActualFromTheoretical( y);
	double rightx  = coord.getXActualFromTheoretical( x + w);
	double topy    = coord.getYActualFromTheoretical( y + h);
	double actuw   = rightx - leftx; 
	double actuh   = bottomy - topy;

	std::stringstream ss;

	ss << "<rect" << " ";

	ss <<
		R"(x=")"      << leftx << R"(")" << " " << 
		R"(y=")"      << topy  << R"(")" << " " << 
		R"(width=")"  << actuw << R"(")" << " " << 
		R"(height=")" << actuh << R"(")" << " ";
	
	ss << getBasicShapeAttr() << ">";

	int animatorlen = animatorpvec.size();
	if ( animatorlen > 0){

		ss << std::endl;

		for ( int i = 0; i < animatorlen; i++){
			ss << animatorpvec[ i]->getContent( *this, coord);
			ss << std::endl;
		}

	}

	ss << "</rect>";

	return ss.str();

}

// 長方形が下から伸びてくるアニメーションを追加する。
void
GraphRect :: 
addAnimateBuildup( double sec)
{

	// BuildupGraphRectAnimatorのコンストラクタを呼び、
	// インスタンスをつくってunique_ptrにして、それをvectorに追加。
	animatorpvec.push_back( std::make_unique <BuildupGraphRectAnimator> ( sec));
	
}


/* ***** class GraphLine ***** */

GraphLine :: 
GraphLine( double xa, double ya, double xb, double yb)
: GraphBasicShape(), x1( xa), y1( ya), x2( xb), y2( yb) // , animatorpvec()
{}

GraphLine :: 
~GraphLine( void)
{}

std::string 
GraphLine :: 
getContent( const Coordinates &coord) 
const
{

	double actux1 = coord.getXActualFromTheoretical( x1);
	double actuy1 = coord.getYActualFromTheoretical( y1);
	double actux2 = coord.getXActualFromTheoretical( x2);
	double actuy2 = coord.getYActualFromTheoretical( y2);

	std::stringstream ss;

	ss << "<line" << " ";

	ss <<
		R"(x1=")" << actux1 << R"(")" << " " << 
		R"(y1=")" << actuy1 << R"(")" << " " << 
		R"(x2=")" << actux2 << R"(")" << " " << 
		R"(y2=")" << actuy2 << R"(")" << " ";
	
	ss << getBasicShapeAttr() << ">";

// ↓まだ。
//	int animatorlen = animatorpvec.size();
//	if ( animatorlen > 0){
//
//		ss << std::endl;
//
//		for ( int i = 0; i < animatorlen; i++){
//			ss << animatorpvec[ i]->getContent( *this, coord);
//			ss << std::endl;
//		}
//
//	}

	ss << "</line>";

	return ss.str();

}

/* ***** class GraphDot ***** */

GraphDot :: 
GraphDot( double cx0, double cy0, double size0)
: GraphBasicShape(), cx( cx0), cy( cy0), size( size0)
{}

GraphDot :: 
~GraphDot( void)
{}

std::string 
GraphDot :: 
getContent( const Coordinates &coord) 
const
{

	double actux = coord.getXActualFromTheoretical( cx);
	double actuy = coord.getYActualFromTheoretical( cy);

	std::stringstream ss;

	ss << "<circle" << " ";

	ss <<
		R"(cx=")" << actux << R"(")" << " " << 
		R"(cy=")" << actuy << R"(")" << " " << 
		R"(r=")"  << size << R"(")" << " ";
	
	ss << getBasicShapeAttr() << ">";

// ↓まだ。
//	int animatorlen = animatorpvec.size();
//	if ( animatorlen > 0){
//
//		ss << std::endl;
//
//		for ( int i = 0; i < animatorlen; i++){
//			ss << animatorpvec[ i]->getContent( *this, coord);
//			ss << std::endl;
//		}
//
//	}

	ss << "</circle>";

	return ss.str();

}


/* ***** class GraphEllipse ***** */

GraphEllipse :: 
GraphEllipse( double cx0, double cy0, double rx0, double ry0)
: GraphBasicShape(), cx( cx0), cy( cy0), rx( rx0), ry( ry0)
{}

GraphEllipse :: 
~GraphEllipse( void)
{}

std::string 
GraphEllipse :: 
getContent( const Coordinates &coord) 
const
{

	double actucx = coord.getXActualFromTheoretical( cx);
	double actucy = coord.getYActualFromTheoretical( cy);

	double actucx2 = coord.getXActualFromTheoretical( cx + rx);
	double acturx = actucx2 - actucx;

	double actucy2 = coord.getYActualFromTheoretical( cy - ry);
	double actury = actucy2 - actucy;

	std::stringstream ss;

	ss << "<ellipse" << " ";

	ss <<
		R"(cx=")" << actucx << R"(")" << " " << 
		R"(cy=")" << actucy << R"(")" << " " << 
		R"(rx=")" << acturx << R"(")" << " " << 
		R"(ry=")" << actury << R"(")" << " ";
	
	ss << getBasicShapeAttr() << ">";

	ss << "</ellipse>";

	return ss.str();

}


/* ***** class BuildupGraphRectAnimator ***** */

BuildupGraphRectAnimator ::
BuildupGraphRectAnimator( double s0)
: sec( s0)
{}

BuildupGraphRectAnimator ::
~BuildupGraphRectAnimator( void)
{}

std::string 
BuildupGraphRectAnimator :: 
getContent( const GraphRect &rect, const Coordinates &coord)
const
{

	double x = rect.getX();
	double y = rect.getY();
	double w = rect.getW();
	double h = rect.getH();

	// below are "actual" coordinate points
	double leftx = coord.getXActualFromTheoretical( x);
	double rightx = coord.getXActualFromTheoretical( x + w);
	double bottomy = coord.getYActualFromTheoretical( y);
	double topy = coord.getYActualFromTheoretical( y + h);

	double actuy_start = bottomy;
	double actuy_end = topy;

	double actuh_start = 0;
	double actuh_end = bottomy - topy;

	std::stringstream ss;

	ss << "<animate " <<
		R"(attributeName=")" << "y"         << R"(")" << " " << 
		R"(begin=")"         << "0s"        << R"(")" << " " << 
		R"(dur=")"           << sec         << R"(")" << " " << 
		R"(from=")"          << actuy_start << R"(")" << " " << 
		R"(to=")"            << actuy_end   << R"(")" << " " << 
		R"(repeatCount=")"   << "1"         << R"(")" << " " << 
	"/>";

	ss << std::endl;

	ss << "<animate " <<
		R"(attributeName=")" << "height"    << R"(")" << " " << 
		R"(begin=")"         << "0s"        << R"(")" << " " << 
		R"(dur=")"           << sec         << R"(")" << " " << 
		R"(from=")"          << actuh_start << R"(")" << " " << 
		R"(to=")"            << actuh_end   << R"(")" << " " << 
		R"(repeatCount=")"   << "1"         << R"(")" << " " << 
	"/>";

	return ss.str();

}


/* ***** class SvgGraphMakerBase ***** */

SvgGraphMakerBase :: 
SvgGraphMakerBase( void)
{}

SvgGraphMakerBase :: 
~SvgGraphMakerBase( void)
{}

void 
SvgGraphMakerBase :: 
setDefaults( void)
{

	graph_title = "(No Title)";
	xaxis_title = "x (Not Labeled)";
	yaxis_title = "y (Not Labeled)";

	// default values
	svgwidth = 500;
	svgheight = 500;
	outermargin = 20;
	graph_title_fontsize = 20;
	graph_title_margin = 10; // グラフタイトルの下のマージン
	axis_title_fontsize = 20;
	axis_title_margin = 5; // 軸タイトルと軸ラベルの間のマージン
	axis_label_fontsize = 14;
	axis_ticklength = 5;

}

void 
SvgGraphMakerBase :: 
modifyMetrics( const std::map <string, double> &args)
{

	if ( args.size() < 1){
		return;
	}

	std::map <string, double &> manip = {
		{ "svgwidth",             svgwidth             },
		{ "svgheight",            svgheight            },
		{ "outermargin",          outermargin          },
		{ "graph_title_fontsize", graph_title_fontsize },
		{ "graph_title_margin",   graph_title_margin   },
		{ "axis_title_fontsize",  axis_title_fontsize  },
		{ "axis_title_margin",    axis_title_margin    },
		{ "axis_label_fontsize",  axis_label_fontsize  },
		{ "axis_ticklength",      axis_ticklength      }
	};

	for ( const auto p : manip){
		if ( args.count( p.first) > 0){
			p.second = args.at( p.first);
		}
	}

}

void 
SvgGraphMakerBase :: 
setGraphTitle( const std::string &s)
{
	graph_title = s;
}

void 
SvgGraphMakerBase :: 
setXAxisTitle( const std::string &s)
{
	xaxis_title = s;
}

void 
SvgGraphMakerBase :: 
setYAxisTitle( const std::string &s)
{
	yaxis_title = s;
}


/* ***** class SvgHistogramMaker ***** */

// 階級の左端点のvector、右端点のvector、ケース数のvectorを与える。
// an0はアニメーションにするかどうかを示す。
SvgHistogramMaker :: 
SvgHistogramMaker( 
	const std::vector <double> &lv0,
	const std::vector <double> &rv0,
	const std::vector <int> &c0,
	bool an0 // = false
)
: leftvec( lv0), rightvec( rv0), counts( c0), animated( an0)
{
	setDefaults();
}

// データのベクトルそのものを与える。
// 階級の分け方などは自動でなされる。
SvgHistogramMaker :: 
SvgHistogramMaker(
	const std::vector <double> &data,
	bool an0 // = false
)
: animated( an0)
{

	RecodeTable <double, int> rt;
	rt.setAutoTableFromContVar( data);

	FreqType <int, int> ft;
	ft.setFreqFromRecodeTable( data, rt);

	vector <int> codes; // basically meaningless
	ft.getVectors( codes, counts);

	ft.getRangeVectors( leftvec, rightvec);	

	setDefaults();

}

SvgHistogramMaker :: 
~SvgHistogramMaker( void)
{}

SvgGraph
SvgHistogramMaker :: 
createGraph( void)
{

	SvgGraph svgg( svgwidth, svgheight, 0, 0, svgwidth, svgheight);

	svgg.svgwidth = svgwidth;
	svgg.svgheight = svgheight;
	svgg.outermargin = outermargin;
	svgg.graph_title_fontsize = graph_title_fontsize;
	svgg.graph_title_margin = graph_title_margin;
	svgg.axis_title_fontsize = axis_title_fontsize;
	svgg.axis_title_margin = axis_title_margin;
	svgg.axis_label_fontsize = axis_label_fontsize;
	svgg.axis_ticklength = axis_ticklength;

	double xmin = leftvec.front();
	double xmax = rightvec.back();
	double ymin = 0;
	double ymax = *( max_element( counts.begin(), counts.end()));

	svgg.setDefaultCoordAndGridpoints( xmin, xmax, ymin, ymax);

	svgg.setGraphTitle( graph_title);
	svgg.setXAxisTitle( xaxis_title);
	svgg.setYAxisTitle( yaxis_title);

	svgg.prepareGraph();


	// 描画領域への描画開始

	svgg.startDrawingGraphPane();

	if ( animated == true){
		svgg.drawBins( leftvec, rightvec, counts, "gray", true);
	} else {
		svgg.drawBins( leftvec, rightvec, counts, "gray");
	}

	svgg.endDrawingGraphPane();

	// 描画領域への描画終了

	return svgg;

}


/* ***** class SvgScatterMaker ***** */

SvgScatterMaker :: 
SvgScatterMaker( 
	const std::vector <double> &x0,
	const std::vector <double> &y0
) : xvec( x0), yvec( y0)
{
	setDefaults();
}

SvgScatterMaker :: 
~SvgScatterMaker( void)
{}

SvgGraph
SvgScatterMaker :: 
createGraph( void)
{

	SvgGraph svgg( svgwidth, svgheight, 0, 0, svgwidth, svgheight);

	auto xminmax = std::minmax_element( xvec.begin(), xvec.end());
	double xmin = *( xminmax.first);
	double xmax = *( xminmax.second);

	auto yminmax = std::minmax_element( yvec.begin(), yvec.end());
	double ymin = *( yminmax.first);
	double ymax = *( yminmax.second);

	svgg.setDefaultCoordAndGridpoints( xmin, xmax, ymin, ymax);

	svgg.setGraphTitle( graph_title);
	svgg.setXAxisTitle( xaxis_title);
	svgg.setYAxisTitle( yaxis_title);

	svgg.prepareGraph();


	// 描画領域への描画開始

	svgg.startDrawingGraphPane();

	svgg.drawPoints( xvec, yvec, "black");

	svgg.endDrawingGraphPane();

	// 描画領域への描画終了

	return svgg;

}

#endif // ksvggraph_cpp_include_guard
