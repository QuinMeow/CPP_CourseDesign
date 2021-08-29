#include"CRSImg.h"


void Usage() //命令行提示
{
	cout << "############ Remote Sensing Image Process Tools.############\n";
	cout << "# X – Exit Exit\t退出程序" << endl;
	cout << "# O – Open \t打开影像文件" << endl;
	cout << "# D – Display \t显示影像文件" << endl;
	cout << "# I – Information\t输出当前图像的路径 ，行列值 、波段数 、数据类 数据类 型、排列方式等信息" << endl;
	cout << "# C – Closed\t关闭当前图像" << endl;
	cout << "# S – Statistics\t输出图像数据统计量 ，若文件未打开 ，输出提示" << endl;
	cout << "# H – Histogram\t输出图像的直方" << endl;
	cout << "# ? – Help\t输出本信息" << endl;
	cout << "# A – Save as\t输入保存的文件路径 ，输出图像为二进制文件" << endl;
	cout << "# R – Rotate\t图像旋转，输入角度逆时针" << endl;
	cout << "# Z – Zoom\t图像缩放，输入比例尺出" << endl;
	cout << "# F – Filter\t输入滤波核，输出滤波后图像" << endl;
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
		switch (cmd) //进入菜单
		{
		case 'O':
		case 'o':
			if (image1.IsOpen())
				cerr << "图像已打开" << endl;
			else {
				cout << "输入文件路径" << endl;
				string path;
				cin >> path;
				if (image1.Open((char*)path.data()))
					cout << "打开成功" << endl;
				else
				{
					cout << "打开失败" << endl;
					image1.~CRSImage();
				}
			}
			break;
		case 'D':
		case 'd':
			if (image1.IsOpen())
				DisplayImage(&image1);
			else
				cerr << "图像未打开" << endl;
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
					cout << "关闭成功" << endl;
			}
			else
				cout << "文件未打开" << endl;
			break;
		case 'S':
		case 's':
			image1.ShowStatasic();
			break;
		case 'H':
		case 'h':
			cout << "输入想要输出直方图的频段（总共" << image1.GetBands() << "个)： ";
			int band;
			cin >> band;
			if (band >= image1.GetBands() || band < 0)
				cout << "输入错误！" << endl;
			else
				image1.ShowHistogram(band);
			break;
		case '?':
			Usage();
			break;
		case 'A':
		case 'a': {
			string path;
			cout << "输入保存的图像文件路径" << endl;
			cin >> path;
			if (image1.SaveToFile(path.c_str()))
				cout << "保存成功" << endl;
			else
				cout << "保存失败" << endl;
			break;
		}
		case 'R':
		case 'r': {
			cout << "输入逆时针旋转角度: ";
			int degree;
			cin >> degree;
			Image_Operation rotate1(image1);
			rotate1.Rotate(degree);
			cout << "是否应用到原图？（y/N）" << endl;
			char cmd;
			cin >> cmd;
			if (cmd == 'y' || cmd == 'Y')
				if (image1.ApplyOperation(rotate1))
					cout << "应用成功!" << endl;
			break;
		}
		case 'Z':
		case 'z': {
			Image_Operation zoom1(image1);
			cout << "当前宽高为" << image1.GetSamples() << "x" << image1.GetLines() << endl;
			cout << "输入缩放后的宽和高: ";
			int destW, destH;
			cin >> destW >> destH;
			zoom1.Zoom(destW, destH);
			cout << "是否应用到原图？（y/N）" << endl;
			char cmd;
			cin >> cmd;
			if (cmd == 'y' || cmd == 'Y')
				if (image1.ApplyOperation(zoom1))
					cout << "应用成功" << endl;
			break;
		}
		case 'F':
		case 'f': {
			int W, type;
			cout << "输入滤波类型（0=加权均值滤波，1=锐化（拉普拉斯）滤波）" << endl;
			cin >> type;
			if (type == 0)
			{
				cout << "输入滤波核大小（>=3,奇数）： " << endl;
				cin >> W;
			}
			else
				W = 3;
			if (!(type == 0 || type == 1) || (W % 2 == 0))
			{
				cerr << "输入错误" << endl;
				break;
			}
			Image_Operation fliter1(image1);
			fliter1.Fliter(W,type);
			cout << "是否应用到原图？（y/N）" << endl;
			char cmd;
			cin >> cmd;
			if (cmd == 'y' || cmd == 'Y')
				if (image1.ApplyOperation(fliter1))
					cout << "应用成功!" << endl;
			break;
		}
		case 'X':
		case 'x':
			cout << "退出" << endl;
			break;
		default:
			cout << "非法指令" << endl;
			break;
		}
	} while (cmd != 'X' && cmd != 'x');

	return 0;
}