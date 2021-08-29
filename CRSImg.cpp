{
	if (str.empty())
	{
		return str;
	}
	str.erase(0, str.find_first_not_of(" "));
	str.erase(str.find_last_not_of(" ") + 1);
	return str;
}

void DisplayImage(CRSImage * pRSImg)
{
	// �����ж�
	if (pRSImg == NULL)
		return;

	int		i, j;
	int		rgb[3], bottomB[3], topB[3];
	int		Coefficient; //��������ϵ��
	int		nBands = pRSImg->GetBands();		//����
	int		nSamples = pRSImg->GetSamples();	//��
	int		nLines = pRSImg->GetLines();		//��
	unsigned char***	pppData = pRSImg->GetDataBuffer();	//��ά����ͷָ��
	do
	{
		cout << "������Ϊrgbͨ���Ĳ�����ţ���ǰ����" << nBands << "�����Σ���" << endl;
		cin >> rgb[0] >> rgb[1] >> rgb[2];
		if (!(rgb[0] >= 0 && rgb[0] <= nBands && rgb[1] >= 0 && rgb[1] <= nBands && rgb[2] >= 0 && rgb[2] <= nBands))
			cerr << "�������������ѡ��" << endl;
	} while (!(rgb[0] >= 0 && rgb[0] <= nBands && rgb[1] >= 0 && rgb[1] <= nBands && rgb[2] >= 0 && rgb[2] <= nBands));

	do
	{
		cout << "�Ƿ���������������죿(0-�����죬1-�������죬2-�߶Աȣ�2%������)" << endl;
		cin >> Coefficient;
		if (!(Coefficient == 0 || Coefficient == 1 || Coefficient == 2))
			cerr << "�������������ѡ��" << endl;
	} while (!(Coefficient == 0 || Coefficient == 1 || Coefficient == 2));
	
	//�߶Ա��������2%λ
	if (Coefficient == 2)
	{
		long *Npts, *Total;
		long Pixels = nLines * nSamples;
		bool flagB, flagT;
		float *Acc_Pct;
		for (int k = 0; k < 3; k++)
		{
			Npts = new long[256](); //��ʼ����̬����
			Total = new long[256]();
			Acc_Pct = new float[256]();
			for (int i = 0; i < nLines; i++)
				for (int j = 0; j < nSamples; j++)
					Npts[static_cast<int>(pppData[i][j][rgb[k]])] += 1; //ͳ��ÿһ���������Ϣ
			flagB = flagT = 1;
			for (int i = 0; i < 256; i++)
			{
				if (i)
					Total[i] = (Npts[i] + Total[i - 1]); //�����ۼ�ֵ
				else
					Total[i] = Npts[i];
				Acc_Pct[i] = static_cast<float>(Total[i]) * 100 / Pixels;
				if (Acc_Pct[i] >= 2.0&&flagB)
				{
					bottomB[k] = i;
					flagB = false;
				}
				else if (Acc_Pct[i] >= 98.0&&flagT)
				{
					topB[k] = i;
					flagT = false;
					break;
				}
			}
			delete[] Npts, Total, Acc_Pct;
		}
	}

	// ����EasyX IMAGE����
	initgraph(nSamples, nLines);
	IMAGE	img(nSamples, nLines);
	DWORD*	pImgBuffer = GetImageBuffer(&img);

	// �������أ���ʾͼ��
	for (i = 0; i < nLines; i++)
		for (j = 0; j < nSamples; j++)
			pImgBuffer[i*nSamples + j] = RGB(MapBirghtness(pppData, i, j, rgb[0], Coefficient, bottomB[0], topB[0]), MapBirghtness(pppData, i, j, rgb[1], Coefficient, bottomB[1], topB[1]), MapBirghtness(pppData, i, j, rgb[2], Coefficient, bottomB[2], topB[2]));

	// ����Ӱ��
	putimage(0, 0, &img);

	// �ȴ��û��� ESC���˳���ʾ����
	while (!_kbhit())
	{
		if (_getch() == 27)
			break;
	}

	// �رջ��ƴ���
	closegraph();
}


bool CRSImage::ReadMetaData(const char* lpstrMetaFilePath)  //��ȡ�ļ�Ԫ����
{
	ifstream    ifs;		//input file stream
	string      strLine;
	string      strSubTxt;
	stringstream    ss;		//string stream

	char         sBuff[260];

	ifs.open(lpstrMetaFilePath, ios_base::in); //��ֻ����ʽ��Ԫ�����ļ�
	if (!ifs.is_open())
		return false;

	while (!ifs.eof())   //end of file
	{
		ifs.getline(sBuff, 260);
		strLine = sBuff;

		ss.clear();
		ss.str(strLine); //"samples = 640"
		getline(ss, strSubTxt, '=');
		ClearHeadTailSpace(strSubTxt);
		if (strSubTxt == "description")
		{
			getline(ss, strSubTxt, '{');
			getline(ifs, m_sDescription, '}');
			getline(ifs, strSubTxt, '\n');
		}
		else if (strSubTxt == "samples")
		{
			ss >> m_nSamples;
		}
		else if (strSubTxt == "lines")
		{
			ss >> m_nLines;
		}
		else if (strSubTxt == "bands")
		{
			ss >> m_nBands;
			m_fWaveLength = new float[m_nBands];
			m_sBandNames = new string[m_nBands];
		}
		else if (strSubTxt == "header offset")
		{
			ss >> m_nHeaderOffset;
		}
		else if (strSubTxt == "data type")
		{
			ss >> m_nDataType;
		}
		else if (strSubTxt == "file type")
		{
			getline(ss, m_sFileType, '\n');
		}
		else if (strSubTxt == "interleave") //�ж�դ����������
		{
			ss >> strSubTxt;
			ClearHeadTailSpace(strSubTxt);
			if (strSubTxt == "bsq")
			{
				m_eInterleave = BSQ;
			}
			else if (strSubTxt == "bip")
			{
				m_eInterleave = BIP;
			}
			else if (strSubTxt == "bil")
			{
				m_eInterleave = BIL;
			}
		}
		else if (strSubTxt == "sensor type")
		{
			getline(ss, m_sSensorType, '\n');
			ClearHeadTailSpace(m_sSensorType);
		}
		else if (strSubTxt == "byte order")
		{
			ss >> m_nByteOrder;
		}
		else if (strSubTxt == "wavelength units")
		{
			getline(ss, m_sWavelengthUnits, '\n');
			ClearHeadTailSpace(m_sWavelengthUnits);
		}
		else if (strSubTxt == "z plot range")
		{
			ss >> m_fZPlotRange[0] >> m_fZPlotRange[1];
		}
		else if (strSubTxt == "z plot titles")
		{
			getline(ss, strSubTxt, '{');
			getline(ss, m_sZPlotTitles[0], ',');
			getline(ss, m_sZPlotTitles[1], '}');
		}
		else if (strSubTxt == "default stretch")
		{
			getline(ss, m_sDefaultStretch, '\n');
			ClearHeadTailSpace(m_sDefaultStretch);
		}
		else if (strSubTxt == "band names")
		{
			getline(ss, strSubTxt, '{');
			for (int i = 0; i < m_nBands - 1; i++)
			{
				getline(ifs, m_sBandNames[i], ',');
				ClearHeadTailSpace(m_sBandNames[i]);
			}
			getline(ifs, m_sBandNames[m_nBands - 1], '}');
			ClearHeadTailSpace(m_sBandNames[m_nBands - 1]);
		}
		else if (strSubTxt == "wavelength")
		{
			string tmp;
			getline(ss, strSubTxt, '{');
			for (int i = 0; i < m_nBands - 1; i++)
			{
				getline(ifs, tmp, ',');
				ClearHeadTailSpace(tmp);
				m_fWaveLength[i] = stof(tmp);
			}
			getline(ifs, tmp, '}');
			ClearHeadTailSpace(tmp);
			m_fWaveLength[m_nBands - 1] = stof(tmp);
		}
	}

	return true;
}

bool CRSImage::InitBuffer(unsigned char ***&pppData, int maxRow, int maxCol, int maxBand)
{
	if (pppData != nullptr)
		return false;
	pppData = new unsigned char**[maxRow];
	for (int i = 0; i < maxRow; i++)
	{
		pppData[i] = new unsigned char*[maxCol];
		for (int j = 0; j < maxCol; j++)
			pppData[i][j] = new unsigned char[maxBand];
	}
	return true;
}

bool CRSImage::ClearBuffer(unsigned char ***& pppData, int maxRow, int maxCol)
{
	if (pppData != nullptr)
	{
		//ɾ��ԭm_pppData
		for (int i = 0; i < maxRow; i++)
		{
			for (int j = 0; j < maxCol; j++)
				delete[] pppData[i][j];
			delete[] pppData[i];
		}
		delete[] pppData;
		pppData = nullptr;
		return true;
	}
	else
		return false;
}

bool CRSImage::ReadImgData(const char * lpstrImgFilePath) //��ȡͼ������
{
	ifstream ifs;

	ifs.open(lpstrImgFilePath, ios_base::binary);
	if (!ifs.is_open())
		return false;

	switch (m_eInterleave)
	{
	case BIL:
		for (int i = 0; i < m_nLines; i++)
			for (int k = 0; k < m_nBands; k++)
				for (int j = 0; j < m_nSamples; j++)
					ifs.read(reinterpret_cast<char*>(&m_pppData[i][j][k]), sizeof(unsigned char));
		break;
	case BIP:
		for (int i = 0; i < m_nLines; i++)
			for (int j = 0; j < m_nSamples; j++)
				ifs.read(reinterpret_cast<char*>(m_pppData[i][j]), sizeof(unsigned char)*m_nBands);
		break;
	case BSQ:
		for (int k = 0; k < m_nBands; k++)
			for (int i = 0; i < m_nLines; i++)
				for (int j = 0; j < m_nSamples; j++)
					ifs.read(reinterpret_cast<char*>(&m_pppData[i][j][k]), sizeof(unsigned char));
		break;
	default:
		return false;
	}
	return true;
}

int CRSImage::CountMin(int band) const
{
	int Min = 32767;
	for (int i = 0; i < m_nLines; i++)
		for (int j = 0; j < m_nSamples; j++)
			if (m_pppData[i][j][band] < Min)
				Min = m_pppData[i][j][band];
	return Min;
}

int CRSImage::CountMax(int band) const
{
	int Max = -1;
	for (int i = 0; i < m_nLines; i++)
		for (int j = 9; j < m_nSamples; j++)
			if (m_pppData[i][j][band] > Max)
				Max = m_pppData[i][j][band];
	return Max;
}

float CRSImage::CountMean(int band) const
{
	float sum = 0;
	for (int i = 0; i < m_nLines; i++)
		for (int j = 9; j < m_nSamples; j++)
			sum += m_pppData[i][j][band];
	return sum / CountPixel();
}

float CRSImage::CountStdev(int band) const
{
	float Mean = CountMean(band);
	float ss = 0.0;
	for (int i = 0; i < m_nLines; i++)
		for (int j = 9; j < m_nSamples; j++)
			ss += pow(m_pppData[i][j][band] - Mean, 2);
	float var_k = ss / (CountPixel() - 1);
	return sqrt(var_k);
}

int MapBirghtness(unsigned char ***& pppData, int i, int j, int k, int Coefficient, int bottomB, int topB)
{
	switch (Coefficient)
	{
	case 0:
		if (pppData[i][j][k] < 0)
			return 0;
		else if (pppData[i][j][k] > 255)
			return 255;
		else
			return pppData[i][j][k];
		break;
	case 1:
		if (2.55*pppData[i][j][k] < 0)
			return 0;
		else if (2.55*pppData[i][j][k] > 255)
			return 255;
		else
			return static_cast<int>(2.55*pppData[i][j][k]);
		break;
	case 2:
	{
		float ratio = float(255.0) / static_cast<float>(topB - bottomB);
		if (pppData[i][j][k] < bottomB)
			return 0;
		else if (pppData[i][j][k] > topB)
			return 255;
		else
			return static_cast<int>(ratio*(pppData[i][j][k]-bottomB));
		break;
	}
	default:
		return -1;
	}
}

CRSImage::CRSImage()
{
	m_pppData = nullptr;
	m_fWaveLength = nullptr;
	m_sBandNames = nullptr;
	m_nBands = -1;
	m_nLines = -1;
	m_nSamples = -1;
	m_nDataType = -1;
	m_nByteOrder = -1;
	m_nHeaderOffset = -1;
	m_fZPlotRange[0] = -1;

}

CRSImage::CRSImage(unsigned char *** pppData, int Lines, int Samples, int Bands) :m_nLines(Lines), m_nSamples(Samples), m_nBands(Bands)
{

	m_pppData = new unsigned char**[m_nLines];
	for (int i = 0; i < m_nLines; i++)
	{
		m_pppData[i] = new unsigned char*[m_nSamples];
		for (int j = 0; j < m_nSamples; j++)
		{
			m_pppData[i][j] = new unsigned char[m_nBands];
			for (int k = 0; k < m_nBands; k++)
				m_pppData[i][j][k] = pppData[i][j][k];
		}
	}
	m_fWaveLength = nullptr;
	m_sBandNames = nullptr;
}

CRSImage::~CRSImage()
{
	if (this->IsOpen())
	{
		ClearBuffer(m_pppData, m_nLines, m_nSamples);
		if (m_fWaveLength != nullptr)
		{
			delete[] m_fWaveLength;
			m_fWaveLength = nullptr;
		}
		if (m_sBandNames != nullptr)
		{
			delete[] m_sBandNames;
			m_sBandNames = nullptr;
		}
	}
}

bool CRSImage::Open(const char* lpstrPath)
{
	if (NULL == lpstrPath)
		return false;

	// 1. Read Meta Data
	string	strMetaFilePath = lpstrPath;
	int	pos = strMetaFilePath.rfind('.'); //��λ��׺����ʼλ
	if (pos >= 0)
	{
		strMetaFilePath = strMetaFilePath.substr(0, pos); //���㵽��׺����Ƭ
	}
	strMetaFilePath.append(".hdr"); //����hdr��׺
	if (!ReadMetaData(strMetaFilePath.c_str())) //���ò���ȡԪ����
	{
		cerr << "Read Meta Data Failed." << endl;
		return false;
	}

	// 2. Initialize Buffer
	if (!InitBuffer(m_pppData, m_nLines, m_nSamples, m_nBands))
	{
		cerr << "Initialize Buffer Failed." << endl;
		return false;
	}

	// 3. Read File
	if (!ReadImgData(lpstrPath))
	{
		cerr << "Read Image Data Failed." << endl;
		return false;
	}

	return true;
}

void CRSImage::ShowInformation() const
{
	cout << "ENVI" << endl;
	if (m_sDefaultStretch != "")
		cout << "description = {\n" << m_sDescription << '}' << endl;
	cout << "samples = " << m_nSamples << endl;
	cout << "lines = " << m_nLines << endl;
	cout << "bands = " << m_nBands << endl;
	cout << "header offset = " << m_nHeaderOffset << endl;
	cout << "file type = " << m_sFileType << endl;
	cout << "data type = " << m_nDataType << endl;
	cout << "interleave = ";
	switch (m_eInterleave)
	{
	case CRSImage::BSQ:
		cout << "BSQ" << endl;
		break;
	case CRSImage::BIP:
		cout << "BIP" << endl;
		break;
	case CRSImage::BIL:
		cout << "BIL" << endl;
		break;
	default:
		break;
	}
	if (m_sSensorType != "")
		cout << "sensor typr = " << m_sSensorType << endl;
	cout << "byte order = " << m_nByteOrder << endl;
	if (m_sWavelengthUnits != "")
		cout << "wavelength units = " << m_sWavelengthUnits << endl;
	if (m_fZPlotRange[0] != -1)
		cout << "z plot range = {" << m_fZPlotRange[0] << ", " << m_fZPlotRange[1] << '}' << endl;
	if (m_sZPlotTitles[0] != "")
		cout << "z plot titles = {" << m_sZPlotTitles[0] << ", " << m_sZPlotTitles[1] << '}' << endl;
	if (m_sDefaultStretch != "")
		cout << "default stretch = " << m_sDefaultStretch << endl;
	if (m_sBandNames != nullptr)
	{
		cout << "band names = {" << endl;
		for (int i = 0; i < m_nBands - 1; i++)
			cout << m_sBandNames[i] << ", ";
		cout << m_sBandNames[m_nBands - 1] << '}' << endl;
	}
	if (m_fWaveLength != nullptr)
	{
		cout << "wavelength = {" << endl;
		for (int i = 0; i < m_nBands - 1; i++)
			cout << m_fWaveLength[i] << ", ";
		cout << m_fWaveLength[m_nBands - 1] << '}' << endl;
	}

}

void CRSImage::ShowStatasic() const
{
	cout << "Pixels: " << CountPixel() << " Points" << endl;
	cout << "Basic Stats	  Min	  Max	     Mean	    Stdev" << endl;
	for (int i = 0; i < m_nBands; i++)
	{
		cout << setw(11) << setfill(' ') << m_sBandNames[i] << '\t'
			<< setw(5) << CountMin(i) << '\t'
			<< setw(5) << CountMax(i) << '\t'
			<< setiosflags(ios::fixed) << setprecision(6) << setw(9) << CountMean(i) << '\t'
			<< setw(9) << CountStdev(i) << endl;
	}

}

void CRSImage::ShowHistogram(int band) const
{
	long Pixels = CountPixel();
	long *Npts, *Total;
	float *Percent, *Acc_Pct;
	int k = band;
	Npts = new long[101](); //��ʼ����̬����
	Total = new long[101]();
	Percent = new float[101]();
	Acc_Pct = new float[101]();
	for (int i = 0; i < m_nLines; i++)
		for (int j = 0; j < m_nSamples; j++)
			if(static_cast<int>(m_pppData[i][j][k])<=100&& static_cast<int>(m_pppData[i][j][k])>0)
				Npts[static_cast<int>(m_pppData[i][j][k])] += 1; //ͳ��ÿһ���������Ϣ
	cout << "Histogram  	   DN	   Npts	  Total	 Percent	 Acc Pct" << endl;
	for (int i = 0; i < 101; i++)
	{
		if (i)
			Total[i] = (Npts[i] + Total[i - 1]); //�����ۼ�ֵ
		else
			Total[i] = Npts[i];
		Percent[i] = static_cast<float>(Npts[i]) * 100 / Pixels;
		Acc_Pct[i] = static_cast<float>(Total[i]) * 100 / Pixels;
		if (i == 0)
			cout << resetiosflags(ios::right) << setiosflags(ios::left) //���������
			<< setw(11) << setfill(' ') << m_sBandNames[k] << '\t';
		else
			cout << setw(11) << setfill(' ') << '\t';
		cout << resetiosflags(ios::left) << setiosflags(ios::right) //�����Ҷ���
			<< setw(5) << setfill(' ') << i << '\t'
			<< setw(7) << Npts[i]
			<< setw(8) << Total[i] << setiosflags(ios::fixed) << setprecision(4)
			<< setw(9) << Percent[i] << '\t'
			<< setw(8) << Acc_Pct[i] << endl;
	}

	//EasyX���
	initgraph(540, 340);//����ͼ�ν���
	line(20, 320, 20, 20);//������
	line(20, 320, 520, 320);//������
	for (int i = 0; i < 100; i++)
		solidrectangle(20 + 5 * i, 320 - int(ceil(300 * Percent[i] / 10)), 20 + 5 * i + 5, 320);//ͨ�������������Ƴ�ֱ��ͼ

	while (!_kbhit())
	{
		if (_getch() == 27)
			break;
	}

	// �رջ��ƴ���
	closegraph();

	delete[] Npts, Total, Percent, Acc_Pct;
	//}
}

bool CRSImage::ApplyOperation(CRSImage & p)
{
	if (m_nLines != p.m_nLines || m_nSamples != p.m_nSamples)
	{
		if (this->IsOpen())
		{	//�ͷ�
			for (int i = 0; i < m_nLines; i++)
			{
				for (int j = 0; j < m_nSamples; j++)
					delete[] m_pppData[i][j];
				delete[] m_pppData[i];
			}
			delete[] m_pppData;
			m_pppData = nullptr;
			m_nLines = p.m_nLines;
			m_nSamples = p.m_nSamples;
			//�½�
			if (!InitBuffer(m_pppData, m_nLines, m_nSamples, m_nBands))
				return false;
		}
	}
	for (int i = 0; i < m_nLines; i++)
		for (int j = 0; j < m_nSamples; j++)
			for (int k = 0; k < m_nBands; k++)
				m_pppData[i][j][k] = p.m_pppData[i][j][k];
	return true;
}

bool CRSImage::SaveToFile(const char * FilePath)
{
	ofstream ofs;
	string	strMetaFilePath = FilePath;
	int	pos = strMetaFilePath.rfind('.'); //��λ��׺����ʼλ
	if (pos >= 0)
	{
		strMetaFilePath = strMetaFilePath.substr(0, pos); //���㵽��׺����Ƭ
	}
	strMetaFilePath.append(".hdr"); //����hdr��׺

	//д��ͼ���ļ�
	ofs.open(FilePath, ios::binary);
	if (!ofs)
	{
		cerr << "Can't Open!" << endl;
		return false;
	}
	else
	{
		switch (m_eInterleave)
		{
		case CRSImage::BSQ:
			for (int k = 0; k < m_nBands; k++)
				for (int i = 0; i < m_nLines; i++)
					for (int j = 0; j < m_nSamples; j++)
						ofs.write(reinterpret_cast<char*>(&m_pppData[i][j][k]), sizeof(unsigned char));
			break;
		case CRSImage::BIP:
			for (int i = 0; i < m_nLines; i++)
				for (int j = 0; j < m_nSamples; j++)
					ofs.write(reinterpret_cast<char*>(m_pppData[i][j]), sizeof(unsigned char)*m_nBands);
			break;
		case CRSImage::BIL:
			for (int i = 0; i < m_nLines; i++)
				for (int k = 0; k < m_nBands; k++)
					for (int j = 0; j < m_nSamples; j++)
						ofs.write(reinterpret_cast<char*>(&m_pppData[i][j][k]), sizeof(unsigned char));
			break;
		default:
			cerr << "��ʽ�쳣" << endl;
			return false;
			break;
		}
		ofs.close();
	}

	//д��Ԫ�����ļ�
	ofs.open(strMetaFilePath.c_str(), ios::out);
	if (!ofs.is_open())
	{
		cerr << "Can't Open Meta File" << endl;
		return false;
	}
	else
	{
		ofs << "ENVI" << endl;
		if (m_sDefaultStretch != "")
			ofs << "description = {\n" << m_sDescription << '}' << endl;
		ofs << "samples = " << m_nSamples << endl;
		ofs << "lines = " << m_nLines << endl;
		ofs << "bands = " << m_nBands << endl;
		ofs << "header offset = " << m_nHeaderOffset << endl;
		ofs << "file type = " << m_sFileType << endl;
		ofs << "data type = " << m_nDataType << endl;
		ofs << "interleave = ";
		switch (m_eInterleave)
		{
		case CRSImage::BSQ:
			ofs << "BSQ" << endl;
			break;
		case CRSImage::BIP:
			ofs << "BIP" << endl;
			break;
		case CRSImage::BIL:
			ofs << "BIL" << endl;
			break;
		default:
			break;
		}
		if (m_sSensorType != "")
			ofs << "sensor typr = " << m_sSensorType << endl;
		ofs << "byte order = " << m_nByteOrder << endl;
		if (m_sWavelengthUnits != "")
			ofs << "wavelength units = " << m_sWavelengthUnits << endl;
		if (m_fZPlotRange[0] != -1)
			ofs << "z plot range = {" << m_fZPlotRange[0] << ", " << m_fZPlotRange[1] << '}' << endl;
		if (m_sZPlotTitles[0] != "")
			ofs << "z plot titles = {" << m_sZPlotTitles[0] << ", " << m_sZPlotTitles[1] << '}' << endl;
		if (m_sDefaultStretch != "")
			ofs << "default stretch = " << m_sDefaultStretch << endl;
		if (m_sBandNames != nullptr)
		{
			ofs << "band names = {" << endl;
			for (int i = 0; i < m_nBands - 1; i++)
				ofs << m_sBandNames[i] << ", ";
			ofs << m_sBandNames[m_nBands - 1] << '}' << endl;
		}
		if (m_fWaveLength != nullptr)
		{
			ofs << "wavelength = {" << endl;
			for (int i = 0; i < m_nBands - 1; i++)
				ofs << m_fWaveLength[i] << ", ";
			ofs << m_fWaveLength[m_nBands - 1] << '}' << endl;
		}
		ofs.close();
	}
	return true;
}

int Image_Operation::GetRGBValue(double row, double col, int band)
{
	if (col >= m_nSamples)
		col = m_nSamples - 1;

	if (col < 0)
		col = 0;

	if (row >= m_nLines)
		row = m_nLines - 1;

	if (row < 0)
		row = 0;

	return m_pppData[int(row)][int(col)][band];
}

int Image_Operation::GetPixelValue(double p[][4], double x, double y)
{
	//����Ȩ��
	a00 = p[1][1];
	a01 = -.5*p[1][0] + .5*p[1][2];
	a02 = p[1][0] - 2.5*p[1][1] + 2 * p[1][2] - .5*p[1][3];
	a03 = -.5*p[1][0] + 1.5*p[1][1] - 1.5*p[1][2] + .5*p[1][3];
	a10 = -.5*p[0][1] + .5*p[2][1];
	a11 = .25*p[0][0] - .25*p[0][2] - .25*p[2][0] + .25*p[2][2];
	a12 = -.5*p[0][0] + 1.25*p[0][1] - p[0][2] + .25*p[0][3] + .5*p[2][0] - 1.25*p[2][1] + p[2][2] - .25*p[2][3];
	a13 = .25*p[0][0] - .75*p[0][1] + .75*p[0][2] - .25*p[0][3] - .25*p[2][0] + .75*p[2][1] - .75*p[2][2] + .25*p[2][3];
	a20 = p[0][1] - 2.5*p[1][1] + 2 * p[2][1] - .5*p[3][1];
	a21 = -.5*p[0][0] + .5*p[0][2] + 1.25*p[1][0] - 1.25*p[1][2] - p[2][0] + p[2][2] + .25*p[3][0] - .25*p[3][2];
	a22 = p[0][0] - 2.5*p[0][1] + 2 * p[0][2] - .5*p[0][3] - 2.5*p[1][0] + 6.25*p[1][1] - 5 * p[1][2] + 1.25*p[1][3] + 2 * p[2][0] - 5 * p[2][1] + 4 * p[2][2] - p[2][3] - .5*p[3][0] + 1.25*p[3][1] - p[3][2] + .25*p[3][3];
	a23 = -.5*p[0][0] + 1.5*p[0][1] - 1.5*p[0][2] + .5*p[0][3] + 1.25*p[1][0] - 3.75*p[1][1] + 3.75*p[1][2] - 1.25*p[1][3] - p[2][0] + 3 * p[2][1] - 3 * p[2][2] + p[2][3] + .25*p[3][0] - .75*p[3][1] + .75*p[3][2] - .25*p[3][3];
	a30 = -.5*p[0][1] + 1.5*p[1][1] - 1.5*p[2][1] + .5*p[3][1];
	a31 = .25*p[0][0] - .25*p[0][2] - .75*p[1][0] + .75*p[1][2] + .75*p[2][0] - .75*p[2][2] - .25*p[3][0] + .25*p[3][2];
	a32 = -.5*p[0][0] + 1.25*p[0][1] - p[0][2] + .25*p[0][3] + 1.5*p[1][0] - 3.75*p[1][1] + 3 * p[1][2] - .75*p[1][3] - 1.5*p[2][0] + 3.75*p[2][1] - 3 * p[2][2] + .75*p[2][3] + .5*p[3][0] - 1.25*p[3][1] + p[3][2] - .25*p[3][3];
	a33 = .25*p[0][0] - .75*p[0][1] + .75*p[0][2] - .25*p[0][3] - .75*p[1][0] + 2.25*p[1][1] - 2.25*p[1][2] + .75*p[1][3] + .75*p[2][0] - 2.25*p[2][1] + 2.25*p[2][2] - .75*p[2][3] - .25*p[3][0] + .75*p[3][1] - .75*p[3][2] + .25*p[3][3];

	double x2 = x * x;
	double x3 = x2 * x;
	double y2 = y * y;
	double y3 = y2 * y;
	double pixelValue;
	pixelValue = (a00 + a01 * y + a02 * y2 + a03 * y3) +
		(a10 + a11 * y + a12 * y2 + a13 * y3) * x +
		(a20 + a21 * y + a22 * y2 + a23 * y3) * x2 +
		(a30 + a31 * y + a32 * y2 + a33 * y3) * x3;
	return pixelValue < 0 ? 0 : pixelValue >100 ? 100 : (int)pixelValue;
}

unsigned char Image_Operation::ReScale(double i, double t, double j, double u, int k)
{
	//����Χ16������ز���
	for (int x = 0; x < 4; x++)
		for (int y = 0; y < 4; y++)
			tempPixels[x][y] = GetRGBValue(i - 1 + x, j - 1 + y, k);

	return static_cast<unsigned char>(GetPixelValue(tempPixels, t, u));
}

unsigned char Image_Operation::GetFliteredValue(int i, int j, int k, int W)
{
	double pixelValue = 0;
	for (int y = 0; y < W; y++)
		for (int x = 0; x < W; x++)
			pixelValue += fliterMatrix[y][x] * GetRGBValue(i - WOff + y, j - WOff + x, k);
	if (pixelValue > 255)
		return 255;
	else if (pixelValue < 0)
		return 0;
	else
		return static_cast<unsigned char>(pixelValue);
}

void Image_Operation::NewMatrix_Zoom(int startH, int endH, int startW, int endW)
{
	for (int row = startH; row < endH; row++)
	{
		double srcRow = float(row)*rowRatio; //ԴĿ�������
		double i = floor(srcRow);
		double t = srcRow - i;
		for (int col = startW; col < endW; col++)
		{
			double srcCol = float(col)*colRatio; //ԴĿ�������
			double j = floor(srcCol);
			double u = srcCol - j;
			for (int k = 0; k < m_nBands; k++)
				outputImg[row][col][k] = ReScale(i, t, j, u, k);
		}
	}
}

void Image_Operation::NewMatrix_Rotate(int startH, int endH, int startW, int endW)
{
	for (int row = startH; row < endH; row++)
	{
		for (int col = startW; col < endW; col++)
		{
			srcRow = si * double(col - colOff) + co * double(row - rowOff) + double(int(m_nLines / 2));
			srcCol = co * double(col - colOff) - si * double(row - rowOff) + double(int(m_nSamples / 2));
			for (int band = 0; band < m_nBands; band++)
				if (srcRow >= 0. && srcRow < m_nLines - 0.5 && srcCol >= 0. && srcCol < m_nSamples - 0.5) //��֤ԭ���ص���ͼ��Χ��
					outputImg[row][col][band] = ReScale(floor(srcRow), srcRow - floor(srcRow), floor(srcCol), srcCol - floor(srcCol), band);
		}
	}
}

void Image_Operation::NewMatrix_Fliter(int startH, int endH, int startW, int endW)
{
	for (int i = startH; i < endH; i++)
		for (int j = startW; j < endW; j++)
			for (int k = 0; k < m_nBands; k++)
				outputImg[i][j][k] = GetFliteredValue(i, j, k, W);
}

void Image_Operation::Zoom(int destWidth, int destHeight)
{
	//�����¾���
	InitBuffer(outputImg, destHeight, destWidth, m_nBands);
	rowRatio = float(m_nLines) / float(destHeight);
	colRatio = float(m_nSamples) / float(destWidth);
	//���̷߳�ʽӳ�����
	//NewMatrix_ReScale(0,destHeight, 0,destWidth); 
	//ʹ�ö��߳�ӳ�����
	thread t1(&Image_Operation::NewMatrix_Zoom, *this, 0, destHeight / 2, 0, destWidth / 2);
	thread t2(&Image_Operation::NewMatrix_Zoom, *this, 0, destHeight / 2, destWidth / 2, destWidth);
	thread t3(&Image_Operation::NewMatrix_Zoom, *this, destHeight / 2, destHeight, 0, destWidth / 2);
	thread t4(&Image_Operation::NewMatrix_Zoom, *this, destHeight / 2, destHeight, destWidth / 2, destWidth);
	t1.join();
	t2.join();
	t3.join();
	t4.join();
	ClearBuffer(m_pppData, m_nLines, m_nSamples);
	m_pppData = outputImg;
	m_nLines = destHeight;
	m_nSamples = destWidth;
	DisplayImage(this);
}

void Image_Operation::Rotate(int degree)
{

	angle = degree * 3.1415926 / 180;
	co = cos(angle);
	si = sin(angle);
	int rotateW = static_cast<int>(m_nSamples * fabs(co) + m_nLines * fabs(si)); //������ת��ĸߺͿ�
	int rotateH = static_cast<int>(m_nSamples * fabs(si) + m_nLines * fabs(co));
	colOff = rotateW / 2; //����ƫ����
	rowOff = rotateH / 2;
	//������ͼ�����
	InitBuffer(outputImg, rotateH, rotateW, m_nBands);
	//���߳�����ӳ��
	//NewMatrix_Rotate(0, rotateH, 0, rotateW);
	//���߳�
	thread t1(&Image_Operation::NewMatrix_Rotate, *this, 0, rotateH / 2, 0, rotateW / 2);
	thread t2(&Image_Operation::NewMatrix_Rotate, *this, 0, rotateH / 2, rotateW / 2, rotateW);
	thread t3(&Image_Operation::NewMatrix_Rotate, *this, rotateH / 2, rotateH, 0, rotateW / 2);
	thread t4(&Image_Operation::NewMatrix_Rotate, *this, rotateH / 2, rotateH, rotateW / 2, rotateW);
	t1.join();
	t2.join();
	t3.join();
	t4.join();
	ClearBuffer(m_pppData, m_nLines, m_nSamples);
	m_pppData = outputImg;
	m_nLines = rotateH;
	m_nSamples = rotateW;
	DisplayImage(this);
}

void Image_Operation::Fliter(int Width, int type = 0)
{
	W = Width;
	WOff = (W - 1) / 2;
	fliterMatrix = new double *[W];
	for (int i = 0; i < W; i++)
		fliterMatrix[i] = new double[W];
	InitBuffer(outputImg, m_nLines, m_nSamples, m_nBands);
	//����ϵ������
	switch (type) {//��Ȩ��ֵ�˲�
	case 0: {
		double coefficientSum = 0.0;
		for (int i = 0; i < W; i++)
			for (int j = 0; j < W; j++)
			{
				fliterMatrix[i][j] = pow(2, WOff - abs(i - WOff) - abs(j - WOff));
				coefficientSum += fliterMatrix[i][j];
			}
		for (int i = 0; i < W; i++)
			for (int j = 0; j < W; j++)
				fliterMatrix[i][j] = fliterMatrix[i][j] / coefficientSum;
		break;
	}
	case 1: {//������˹�˲�
		fliterMatrix[0][0] = 0;
		fliterMatrix[0][1] = -1;
		fliterMatrix[0][2] = 0;
		fliterMatrix[1][0] = -1;
		fliterMatrix[1][1] = 5;
		fliterMatrix[1][2] = -1;
		fliterMatrix[2][0] = 0;
		fliterMatrix[2][1] = -1;
		fliterMatrix[2][2] = 0;
		break;
	}
	}
	//���߳�
	//NewMatrix_Fliter(0, m_nLines, 0, m_nSamples);
	//���߳�
	thread t1(&Image_Operation::NewMatrix_Fliter, *this, 0, m_nLines / 2, 0, m_nSamples / 2);
	thread t2(&Image_Operation::NewMatrix_Fliter, *this, 0, m_nLines / 2, m_nSamples / 2, m_nSamples);
	thread t3(&Image_Operation::NewMatrix_Fliter, *this, m_nLines / 2, m_nLines, 0, m_nSamples / 2);
	thread t4(&Image_Operation::NewMatrix_Fliter, *this, m_nLines / 2, m_nLines, m_nSamples / 2, m_nSamples);
	t1.join();
	t2.join();
	t3.join();
	t4.join();
	//�����ʱ����
	for (int i = 0; i < W; i++)
		delete[] fliterMatrix[i];
	delete[] fliterMatrix;
	ClearBuffer(m_pppData, m_nLines, m_nSamples);
	m_pppData = outputImg;
	DisplayImage(this);
}
