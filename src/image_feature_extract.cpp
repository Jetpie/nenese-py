#include "image_feature_extract.hpp"

/*
 * Default Constructer
 */
ImageCoder::ImageCoder(void)
{
    /* default setting */
    this->setParams(128,128,8,16);

    int descrSize = vl_dsift_get_descriptor_size(this->dsiftFilter);
    int nKeypoints = vl_dsift_get_keypoint_num(this->dsiftFilter);

    cout<< descrSize << "," << nKeypoints << endl;
}

/*
 * Constructer overloading
 */
ImageCoder::ImageCoder(int stdWidth, int stdHeight, int step, int binSize)
{
    setParams(stdWidth,stdHeight,step,binSize);
}
/*
 * Constructer overloading
 */
ImageCoder::ImageCoder(VlDsiftFilter* filter)
{
    this->dsiftFilter = filter;
    // switch off gaussian windowing
    vl_dsift_set_flat_window(dsiftFilter,true);

    // assume that x part equals to y part
    this->stdWidth = filter->imWidth;
    this->stdHeight = filter->imHeight;
    this->step = filter->stepX;
    this->binSize = filter->geom.binSizeX;

}

/*
 * Default Destructor
 */
ImageCoder::~ImageCoder(void){
    vl_dsift_delete(this->dsiftFilter);
}

void
ImageCoder::setParams(int stdWidth, int stdHeight, int step, int binSize)
{
    this->stdWidth = stdWidth;
    this->stdHeight = stdHeight;
    this->step = step;
    this->binSize = binSize;

    if(this->dsiftFilter)
    {

        cout<< "set filter from EXIST"<<endl;
        this->dsiftFilter->imWidth = stdWidth;
        this->dsiftFilter->imHeight = stdHeight;
        VlDsiftDescriptorGeometry geom =
            *vl_dsift_get_geometry(this->dsiftFilter);
        geom.binSizeX = binSize ;
        geom.binSizeY = binSize ;
        vl_dsift_set_geometry(this->dsiftFilter, &geom);
        vl_dsift_set_steps(this->dsiftFilter, step, step);
    }
    else
    {
        cout<< "set filter from NULL"<<endl;

        this->dsiftFilter =
            vl_dsift_new_basic(stdWidth, stdHeight, step, binSize);

        // switch off gaussian windowing
        vl_dsift_set_flat_window(this->dsiftFilter,true);

    }
}


/*
 * decode dense-sift descripter
 * @param srcImg opencv IplImage
 * @param size
 * @param binSize
 * @return float*
 */
float*
ImageCoder::dsiftDescripter(Mat srcImage)
{
    // check if source image is graylevel
    Mat grayImage;
    if (srcImage.channels() != 1)
        cvtColor(srcImage,grayImage,CV_BGR2GRAY);
    else
        grayImage = srcImage;

    // resize image
    Mat stdImage;
    resize(grayImage, stdImage, Size(this->stdWidth,this->stdHeight),
           0, 0, INTER_LINEAR);

    // validate
    if(!stdImage.data)
        return 0;

    // get valid input for dsift process
    vl_sift_pix *imData = new vl_sift_pix[stdImage.rows*stdImage.cols];
    uchar * rowPtr;
    for (int i=0; i<stdImage.rows; i++)
    {
        rowPtr = stdImage.ptr<uchar>(i);
        for (int j=0; j<stdImage.cols; j++)
        {
            imData[i*stdImage.cols+j] = rowPtr[j];
        }
    }

    // process an image data
    vl_dsift_process(this->dsiftFilter,imData);

    int descrSize = vl_dsift_get_descriptor_size(this->dsiftFilter);
    int nKeypoints = vl_dsift_get_keypoint_num(this->dsiftFilter);

    // return the normalized sift descripters
    return this->normalizeSift(this->dsiftFilter->descrs,
                               descrSize*nKeypoints);
}



string
ImageCoder::llcDescripter(Mat srcImage, float *codebook, int ncb, int k)
{
    // compute dsift feature
    float* dsiftDescr = this->dsiftDescripter(srcImage);

    // get sift descripter size and number of keypoints
    int descrSize = vl_dsift_get_descriptor_size(dsiftFilter);
    int nKeypoints = vl_dsift_get_keypoint_num(dsiftFilter);

    // initialize dsift descripters and codebook opencv matrix
    Mat dsiftMat = cv::Mat(nKeypoints, descrSize, CV_32FC1, dsiftDescr);
    Mat cbMat = cv::Mat(ncb , descrSize, CV_32FC1, codebook);

    /*** Step 1 - compute eucliean distance and sort ***/

    // dd - dsift descripter, cb - codebook
    Mat ddnorm2,cbnorm2;
    cv::reduce(dsiftMat.mul(dsiftMat), ddnorm2, 1, CV_REDUCE_SUM);
    cv::reduce(cbMat.mul(cbMat), cbnorm2, 1, CV_REDUCE_SUM);

    Mat sortedIdx;
    cv::sortIdx(
        // euclidean: u^2 + v^2 + 2uv
        repeat(ddnorm2,1,ncb) +
        repeat(cbnorm2.t(),nKeypoints,1) -
        2 * dsiftMat * cbMat.t(),
        // output mat
        sortedIdx,
        CV_SORT_EVERY_ROW | CV_SORT_ASCENDING
        );

    // release memory
    ddnorm2.release(),cbnorm2.release();

    /*** Step 2 - get knn from codebook***/
    float *LLC = new float[ncb];
    Mat knnIdx = sortedIdx(Rect(0,0,k,nKeypoints));
    // cout << "knnIdx:" << knnIdx.rows << "," << knnIdx.cols << endl;
    // cout << knnIdx<<endl;

    Mat diagDist = Mat::eye(k,k,CV_32FC1);
    diagDist = diagDist.mul(1e-4);

    Mat LLCall = Mat::zeros(nKeypoints,ncb,CV_32FC1);
    Mat U = Mat::zeros(k,descrSize,CV_32FC1);
    Mat cov;
    for(int i=0; i<nKeypoints; i++)
    {
        for(int ii=0; ii<k; ii++)
        {
            U.row(ii) = abs(cbMat.row(knnIdx.at<int>(i,ii) ) - dsiftMat.row(i));
        }
        // covariance = U^T * U
        mulTransposed(U, cov , 0);

        Mat cHat;
        solve(cov + diagDist.mul(trace(cov)),Mat::ones(k,1,CV_32FC1),cHat);
        divide(cHat,sum(cHat),cHat);

        for(int j=0; j<k; j++)
            LLCall.at<float>(i,knnIdx.at<int>(i,j)) = cHat.at<float>(j,0);
    }
    cov.release(), U.release(), diagDist.release(), sortedIdx.release();
    transpose(LLCall,LLCall);

    /*** Step3 - get llc descripter***/
    // prune the maximum value for each LLC
    float max;
    float sumSqrt = 0;
    for(int i=0; i<ncb; i++)
    {
        max = LLCall.at<float>(i,0);
        for(int j=1; j<nKeypoints; j++)
        {
            if(LLCall.at<float>(i,j)> max)
                max = LLCall.at<float>(i,j);
        }
        LLC[i] = max;
        sumSqrt = sumSqrt + max * max;

    }

    // normalization
    ostringstream s;
    s << (LLC[0] / sumSqrt);
    sumSqrt = sqrt(sumSqrt);
    for(int i=1; i<ncb; i++)
    {
        s << ",";
        s << (LLC[i] / sumSqrt);
    }
    return s.str();
}

/*
 * Normalization of dense sift desripters
 *
 */
float* ImageCoder::normalizeSift(float *Descriptors, int size)
{
    float *normDesc = new float[size];
    float temp,temp1;                                           //temp1和temp为存储每一个patch对应的128维向量的均方根
    int i,j;

    for(i=0; i<size/128; i++)
        //每128个元素对应一个patch的特征向量
        //分别对每个特征向量进行归一化
    {
        temp = 0;
        temp1 = 0;
        for(j=0; j<128; j++)
            //求均方差
        {
            //printf("%f\n",Descriptors[i*128 + j]);
            temp  = temp  + Descriptors[i*128 + j] * Descriptors[i*128 + j];
        }
        temp = sqrt(temp);

        //对于均方差大于1的向量，做归一化处理
        if(temp > 1)
        {
            for(j=0; j<128; j++)
            {
                normDesc[i*128 + j] = Descriptors[i*128 + j]/temp;
                if(normDesc[i*128 + j] > 0.2)
                {
                    normDesc[i*128 + j] = 0.2;    //消除梯度太大的量，即归一化后大于0.2的量，强制等于0.2
                }
                temp1 = temp1 + normDesc[i*128 + j]*normDesc[i*128 + j];
            }
            temp1 = sqrt(temp1);
            for(j=0; j<128; j++)
                //第二次归一化
            {
                normDesc[i*128 + j] = normDesc[i*128 + j]/temp1;
            }

        }
        else
            //对于均方差小于1的向量，不做归一化处理
        {
            for(j=0; j<128; j++)
            {
                normDesc[i*128 + j] = Descriptors[i*128 + j];
            }
        }
    }

    return normDesc;
}
