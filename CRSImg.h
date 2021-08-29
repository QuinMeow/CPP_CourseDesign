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
	//��Ա����
protected:
	unsigned char***		m_pppData;	//��ά����
	int				m_nBands;			//������
	int				m_nLines;			//����
	int				m_nSamples;			//����
	int				m_nDataType;		//��������
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
	//��Ա����
protected:
	bool	ReadMetaData(const char* lpstrMetaFilePath); //��ȡ�ļ�Ԫ����
	bool	InitBuffer(unsigned char ***&pppData, int maxRow,int maxCol,int maxBand); //��ʼ������
	bool	ClearBuffer(unsigned char ***&pppData, int maxRow, int maxCol);//ɾ���ض�����
	bool	ReadImgData(const char* lpstrImgFilePath); //��ȡ�ļ�����
	long	CountPixel() const { return m_nLines * m_nSamples; } //��ȡ��������
	//����ͳ����
	int		CountMin(int band) const;
	int		CountMax(int band) const;
	float	CountMean(int band) const;
	float	CountStdev(int band) const;
public:
	CRSImage();
	CRSImage(CRSImage &p) :CRSImage(p.GetDataBuffer(), p.GetLines(), p.GetSamples(), p.GetBands()) {}
	CRSImage(unsigned char ***pppData, int Lines, int Samples, int Bands);
	~CRSImage();
	bool	Open(const char* lpstrPath); //��ȡ�ļ�
	bool	IsOpen() { return (m_pppData != nullptr) ? true : false; }
	inline int GetBands() const { return m_nBands; }
	inline int GetSamples() const { return m_nSamples; }
	inline int GetLines() const { return m_nLines; }
	inline unsigned char*** GetDataBuffer() const { return m_pppData; }
	void ShowInformation() const;//����ļ���Ϣ
	void ShowStatasic() const; //���ͳ����Ϣ
	void ShowHistogram(int band) const;//���ֱ��ͼ
	bool ApplyOperation(CRSImage & p);//Ӧ��ͼ�����
	bool SaveToFile(const char* FilePath);//�����ļ�

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
	int GetRGBValue(double row, double col, int band); //��ֹԽ��+����ת��
	int GetPixelValue(double p[][4], double x, double y);//˫��������������ֵ
	unsigned char ReScale(double i, double t, double j, double u, int k); //˫�����ز���
	unsigned char GetFliteredValue(int i, int j, int k, int W);//�����˲�
	void NewMatrix_Zoom(int startH, int endH, int startW, int endW); //��������˫�������������ڶ��߳�
	void NewMatrix_Rotate(int startH, int endH, int startW, int endW);//������ת�����ڶ��߳�
	void NewMatrix_Fliter(int startH, int endH, int startW, int endW);//�����˲������ڶ��߳�

public:
	Image_Operation(CRSImage& pRSImg) :CRSImage(pRSImg) {}
	void Zoom(int destHeight,int destWidth); //����
	void Rotate(int degree);//��ת
	void Fliter(int W, int type); //�˲�
};


string& ClearHeadTailSpace(string &str); //ȥ���ַ�����β�ո�
//ͼ����ʾ
void DisplayImage(CRSImage *pRSImg);
//��������ӳ��
int MapBirghtness(unsigned char ***&pppData, int i, int j, int k, int Coefficient, int bottomB, int topB); 




