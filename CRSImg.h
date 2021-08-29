#include<graphics.h>
#include<iostream>
#include<iomanip>
#include<fstream>
#include<sstream>
#include<string>
#include<conio.h>
#include<thread>
#include<cmath>
using namespace std;

class CRSImage
{
	//成员数据
protected:
	unsigned char***		m_pppData;	//三维数组
	int				m_nBands;			//波段数
	int				m_nLines;			//行数
	int				m_nSamples;			//列数
	int				m_nDataType;		//数据类型
	int				m_nByteOrder;
	int				m_nHeaderOffset;
	float			m_fZPlotRange[2];
	float*			m_fWaveLength;
	string			m_sFileType;
	string			m_sDescription;
	string			m_sSensorType;
	string			m_sWavelengthUnits;
	string			m_sZPlotTitles[2];
	string			m_sDefaultStretch;
	string*			m_sBandNames;
	enum Interleave { BSQ, BIP, BIL };
	Interleave m_eInterleave;
	//成员函数
protected:
	bool	ReadMetaData(const char* lpstrMetaFilePath); //读取文件元数据
	bool	InitBuffer(unsigned char ***&pppData, int maxRow,int maxCol,int maxBand); //初始化矩阵
	bool	ClearBuffer(unsigned char ***&pppData, int maxRow, int maxCol);//删除特定矩阵
	bool	ReadImgData(const char* lpstrImgFilePath); //读取文件数据
	long	CountPixel() const { return m_nLines * m_nSamples; } //获取像素总数
	//计算统计量
	int		CountMin(int band) const;
	int		CountMax(int band) const;
	float	CountMean(int band) const;
	float	CountStdev(int band) const;
public:
	CRSImage();
	CRSImage(CRSImage &p) :CRSImage(p.GetDataBuffer(), p.GetLines(), p.GetSamples(), p.GetBands()) {}
	CRSImage(unsigned char ***pppData, int Lines, int Samples, int Bands);
	~CRSImage();
	bool	Open(const char* lpstrPath); //读取文件
	bool	IsOpen() { return (m_pppData != nullptr) ? true : false; }
	inline int GetBands() const { return m_nBands; }
	inline int GetSamples() const { return m_nSamples; }
	inline int GetLines() const { return m_nLines; }
	inline unsigned char*** GetDataBuffer() const { return m_pppData; }
	void ShowInformation() const;//输出文件信息
	void ShowStatasic() const; //输出统计信息
	void ShowHistogram(int band) const;//输出直方图
	bool ApplyOperation(CRSImage & p);//应用图像更改
	bool SaveToFile(const char* FilePath);//保存文件

};

class Image_Operation :public CRSImage
{
private:
	unsigned char ***outputImg;
	//ReScale
	double tempPixels[4][4];
	double a00, a01, a02, a03;
	double a10, a11, a12, a13;
	double a20, a21, a22, a23;
	double a30, a31, a32, a33;
	//zoom
	float rowRatio,colRatio;
	//rotate
	double angle, co, si, srcCol, srcRow;
	int colOff, rowOff;
	//fliter
	double **fliterMatrix;
	int WOff,W;
protected:
	int GetRGBValue(double row, double col, int band); //防止越界+类型转换
	int GetPixelValue(double p[][4], double x, double y);//双立方：计算像素值
	unsigned char ReScale(double i, double t, double j, double u, int k); //双立方重采样
	unsigned char GetFliteredValue(int i, int j, int k, int W);//进行滤波
	void NewMatrix_Zoom(int startH, int endH, int startW, int endW); //分区矩阵双立方采样，用于多线程
	void NewMatrix_Rotate(int startH, int endH, int startW, int endW);//分区旋转，用于多线程
	void NewMatrix_Fliter(int startH, int endH, int startW, int endW);//分区滤波，用于多线程

public:
	Image_Operation(CRSImage& pRSImg) :CRSImage(pRSImg) {}
	void Zoom(int destHeight,int destWidth); //缩放
	void Rotate(int degree);//旋转
	void Fliter(int W, int type); //滤波
};


string& ClearHeadTailSpace(string &str); //去除字符串首尾空格
//图像显示
void DisplayImage(CRSImage *pRSImg);
//像素亮度映射
int MapBirghtness(unsigned char ***&pppData, int i, int j, int k, int Coefficient, int bottomB, int topB); 




