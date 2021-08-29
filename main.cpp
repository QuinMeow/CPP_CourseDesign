#include"CRSImg.h"


void Usage() //��������ʾ
{
	cout << "############ Remote Sensing Image Process Tools.############\n";
	cout << "# X �C Exit Exit\t�˳�����" << endl;
	cout << "# O �C Open \t��Ӱ���ļ�" << endl;
	cout << "# D �C Display \t��ʾӰ���ļ�" << endl;
	cout << "# I �C Information\t�����ǰͼ���·�� ������ֵ �������� �������� ������ �͡����з�ʽ����Ϣ" << endl;
	cout << "# C �C Closed\t�رյ�ǰͼ��" << endl;
	cout << "# S �C Statistics\t���ͼ������ͳ���� �����ļ�δ�� �������ʾ" << endl;
	cout << "# H �C Histogram\t���ͼ���ֱ��" << endl;
	cout << "# ? �C Help\t�������Ϣ" << endl;
	cout << "# A �C Save as\t���뱣����ļ�·�� �����ͼ��Ϊ�������ļ�" << endl;
	cout << "# R �C Rotate\tͼ����ת������Ƕ���ʱ��" << endl;
	cout << "# Z �C Zoom\tͼ�����ţ���������߳�" << endl;
	cout << "# F �C Filter\t�����˲��ˣ�����˲���ͼ��" << endl;
	cout << "#################################################################" << endl;
}

int main()
{
	CRSImage image1;
	char cmd;
	Usage();
	do
	{
		cin >> cmd;
		switch (cmd) //����˵�
		{
		case 'O':
		case 'o':
			if (image1.IsOpen())
				cerr << "ͼ���Ѵ�" << endl;
			else {
				cout << "�����ļ�·��" << endl;
				string path;
				cin >> path;
				if (image1.Open((char*)path.data()))
					cout << "�򿪳ɹ�" << endl;
				else
				{
					cout << "��ʧ��" << endl;
					image1.~CRSImage();
				}
			}
			break;
		case 'D':
		case 'd':
			if (image1.IsOpen())
				DisplayImage(&image1);
			else
				cerr << "ͼ��δ��" << endl;
			break;
		case 'I':
		case 'i':
			image1.ShowInformation();
			break;
		case 'C':
		case 'c':
			if (image1.IsOpen())
			{
				image1.~CRSImage();
				if (!image1.IsOpen())
					cout << "�رճɹ�" << endl;
			}
			else
				cout << "�ļ�δ��" << endl;
			break;
		case 'S':
		case 's':
			image1.ShowStatasic();
			break;
		case 'H':
		case 'h':
			cout << "������Ҫ���ֱ��ͼ��Ƶ�Σ��ܹ�" << image1.GetBands() << "��)�� ";
			int band;
			cin >> band;
			if (band >= image1.GetBands() || band < 0)
				cout << "�������" << endl;
			else
				image1.ShowHistogram(band);
			break;
		case '?':
			Usage();
			break;
		case 'A':
		case 'a': {
			string path;
			cout << "���뱣���ͼ���ļ�·��" << endl;
			cin >> path;
			if (image1.SaveToFile(path.c_str()))
				cout << "����ɹ�" << endl;
			else
				cout << "����ʧ��" << endl;
			break;
		}
		case 'R':
		case 'r': {
			cout << "������ʱ����ת�Ƕ�: ";
			int degree;
			cin >> degree;
			Image_Operation rotate1(image1);
			rotate1.Rotate(degree);
			cout << "�Ƿ�Ӧ�õ�ԭͼ����y/N��" << endl;
			char cmd;
			cin >> cmd;
			if (cmd == 'y' || cmd == 'Y')
				if (image1.ApplyOperation(rotate1))
					cout << "Ӧ�óɹ�!" << endl;
			break;
		}
		case 'Z':
		case 'z': {
			Image_Operation zoom1(image1);
			cout << "��ǰ���Ϊ" << image1.GetSamples() << "x" << image1.GetLines() << endl;
			cout << "�������ź�Ŀ�͸�: ";
			int destW, destH;
			cin >> destW >> destH;
			zoom1.Zoom(destW, destH);
			cout << "�Ƿ�Ӧ�õ�ԭͼ����y/N��" << endl;
			char cmd;
			cin >> cmd;
			if (cmd == 'y' || cmd == 'Y')
				if (image1.ApplyOperation(zoom1))
					cout << "Ӧ�óɹ�" << endl;
			break;
		}
		case 'F':
		case 'f': {
			int W, type;
			cout << "�����˲����ͣ�0=��Ȩ��ֵ�˲���1=�񻯣�������˹���˲���" << endl;
			cin >> type;
			if (type == 0)
			{
				cout << "�����˲��˴�С��>=3,�������� " << endl;
				cin >> W;
			}
			else
				W = 3;
			if (!(type == 0 || type == 1) || (W % 2 == 0))
			{
				cerr << "�������" << endl;
				break;
			}
			Image_Operation fliter1(image1);
			fliter1.Fliter(W,type);
			cout << "�Ƿ�Ӧ�õ�ԭͼ����y/N��" << endl;
			char cmd;
			cin >> cmd;
			if (cmd == 'y' || cmd == 'Y')
				if (image1.ApplyOperation(fliter1))
					cout << "Ӧ�óɹ�!" << endl;
			break;
		}
		case 'X':
		case 'x':
			cout << "�˳�" << endl;
			break;
		default:
			cout << "�Ƿ�ָ��" << endl;
			break;
		}
	} while (cmd != 'X' && cmd != 'x');

	return 0;
}