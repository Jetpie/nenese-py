#include "file_utility.hpp"
#include "image_feature_extract.hpp"
#include <unistd.h>
#include <ctime>
#include <iomanip>
#include <map>
#include <vector>
#include <algorithm>

int main(int argc, char * argv[])
{
    string images[] = {"test1.jpg","test2.jpg","test3.jpg","test4.jpg","test5.jpg","test6.jpg"};
    string prefix = "/home/bingqingqu/TAOCP/test_images/";
    string codebookPath =  "/home/bingqingqu/TAOCP/test_images/cb1008001019.txt";
    FileUtil util;
    ImageCoder ic;
    float *codebook = new float[128 * 500];

    clock_t s;
    s = clock();
    char delim[2] = ",";
    // load codebook
    util.file2ptr(codebookPath.c_str(), codebook,delim);
    cout << "codebook:" << float(clock() -s) << endl;

    s = clock();
    string result = util.file2str(codebookPath.c_str());
    cout << "file2str:" << float(clock() -s) << endl;
    clock_t start;

    for(int i=0;i<6;i++)
    {
        Mat src_new = imread(prefix+images[i],0);

        // test
        string llc_test;
        start= clock();
        try{
            llc_test = ic.llcDescripter(src_new, codebook, 500, 5);
        }
        catch(...){
            cout << "fail to llc" <<endl;
        }
        cout << "time test:" << float(clock() -start)/CLOCKS_PER_SEC << endl;
        // cout << llc_test<<endl;
    }
    delete codebook;


    // // reset size and binSize
    // int size = 0;
    // int binSize = 16;
    // float * desc_old = new float[size];
    // for(int i=0;i<6;i++)
    // {
    //     size =0;
    //     binSize=16;
    //     Mat src_new = imread(prefix+images[i],0);
    //     IplImage *src_old = cvLoadImage((prefix+images[i]).c_str(), 0);

    //     // test
    //     string llc_test;
    //     start= clock();
    //     try{
    //         llc_test = ic.llcTest(src_new, codebook, 500, 5);
    //     }
    //     catch(...){
    //         cout << "fail to llc" <<endl;
    //     }
    //     cout << "time test:" << float(clock() -start)/CLOCKS_PER_SEC << endl;
    //     cout << llc_test<<endl;

    //     string llc_new;
    //     // new
    //     start= clock();
    //     try{
    //         llc_new = ic.llcDescripter(src_new, codebook, 500, 5);
    //     }
    //     catch(...){
    //         cout << "fail to llc" <<endl;
    //     }
    //     cout << "time new:" << float(clock() -start)/CLOCKS_PER_SEC << endl;
    //     cout << llc_new << endl;


    //     // old
    //     start= clock();
    //     VlDsiftFilter* ft_old = kf.GetDsift(src_old, size, binSize);
    //     desc_old = bf.normalizedSift(ft_old->descrs, size);
    //     string llcstr = kf.llcCompute(const_cast<float*>(desc_old), codebook, 500, size);
    //     cout << "time old:" << float(clock() -start)/CLOCKS_PER_SEC << endl;
    //     cout << llc_new << endl;
    // }
    // delete codebook;
    // delete desc_old;

    // string imgPath = "/home/bingqingqu/TAOCP/test_images/test1.jpg";
    // string imgPath2 = "/home/bingqingqu/TAOCP/test_images/test2.jpg";
    // Mat src_new = imread(imgPath,0);
    // Mat src_new2 = imread(imgPath2,0);
    // IplImage *src_old = cvLoadImage(imgPath.c_str(), 0);
    // IplImage *src_old2 = cvLoadImage(imgPath2.c_str(), 0);

    // similarKeyFunction kf;
    // similarBasicFunction bf;
    // ImageCoder ic;

    // float *codebook = new float[128 * 500];
    // // load codebook
    // bf.read_TXT_file(codebookPath.c_str(), codebook);

    // // reset size and binSize
    // int size = 0;
    // int binSize = 16;
    // // initialize
    // clock_t start_old,start_new;
    // start_new = clock();

    // // float* desc_new=ic.dsiftDescripter(src_new);
    // string llc_new = ic.llcDescripter(src_new, codebook, 500, 5);
    // cout << llc_new << endl;
    // cout << "time new:" << float(clock() -start_new)/CLOCKS_PER_SEC << endl;
    // start_new = clock();

    // // float* desc_new=ic.dsiftDescripter(src_new);
    // string llc_test = ic.llcTest(src_new, codebook, 500, 5);
    // cout << llc_test<<endl;
    // cout << "time test:" << float(clock() -start_new)/CLOCKS_PER_SEC << endl;


    // start_old = clock();
    // VlDsiftFilter* ft_old = kf.GetDsift(src_old, size, binSize);
    // float * desc_old = new float[size];
    // desc_old = bf.normalizedSift(ft_old->descrs, size);
    // cout << "time old:" << float(clock() -start_old)/CLOCKS_PER_SEC << endl;
    // // compute the final feature
    // string llcstr = kf.llcCompute(const_cast<float*>(desc_old), codebook, 500, size);

    // cout << "time old:" << float(clock() -start_old)/CLOCKS_PER_SEC << endl;
    // // cout << llc_new << endl;
    // // cout << llcstr << endl;
    // delete codebook;


    // Eigen tests
    // MatrixXd m(2,2);
    // m(0,0) = 3;
    // m(1,0) = 2.5;
    // m(0,1) = -1;
    // m(1,1) = m(1,0) + m(0,1);
    // std::cout << m << std::endl;

    // ----------------------------------------------------

    // for(int i=0 ;i < 100;i++)
    // {
    //     for(int j=0;j<128;j++)
    //     {
    //         cout << desc_old[i*128+j]-desc_new[i*128+j]<< ",";
    //     }
    //     cout << endl;
    // }
    // delete ft_old;


    // // reset size and binSize
    // size = 0;
    // binSize = 16;
    // start_old = clock();
    // VlDsiftFilter* ft_old2 = kf.GetDsift(src_old2, size, binSize);

    // float * desc_old2 = new float[size];
    // desc_old2 = bf.normalizedSift(ft_old2->descrs, size);
    // cout << "time old:" << float(clock() -start_old) << endl;
    // start_new = clock();
    // float* desc_new2=ic.dsiftDescripter(src_new2);
    // cout << "time new:" << float(clock() -start_new) << endl;
    // for(int i=0 ;i < 100;i++)
    // {
    //     for(int j=0;j<128;j++)
    //     {
    //         cout << desc_old2[i*128+j]-desc_new2[i*128+j]<< ",";
    //     }
    //     cout << endl;
    // }
    // delete ft_old2;

}
