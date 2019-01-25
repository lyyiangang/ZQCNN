#if defined(_WIN32)
#include "ZQ_FaceDatabaseMaker.h"
#include "ZQ_FaceRecognizerArcFaceZQCNN.h"
#include "ZQ_FaceDetectorLibFaceDetect.h"
#include "ZQ_CNN_CompileConfig.h"
#if ZQ_CNN_USE_BLAS_GEMM
#include <openblas\cblas.h>
#pragma comment(lib,"libopenblas.lib")
#elif ZQ_CNN_USE_MKL_GEMM
#include <mkl\mkl.h>
#pragma comment(lib,"mklml.lib")
#else
#pragma comment(lib,"ZQ_GEMM.lib")
#endif
using namespace std;
using namespace ZQ;

bool CropImagesForDatabase(const std::string& src_fold, const std::string& dst_fold, int max_thread_num, bool strict_check)
{
	int real_num_threads = __max(1, __min(max_thread_num, omp_get_num_procs() - 1));

	std::vector<ZQ_FaceDetectorLibFaceDetect> detectors(real_num_threads);
	std::vector<ZQ_FaceRecognizerArcFaceZQCNN> recognizers(real_num_threads);

	for (int i = 0; i < real_num_threads; i++)
	{
		detectors[i].Init();
	}

	std::vector<ZQ_FaceDetector*> ptr_detectors(real_num_threads);
	std::vector<ZQ_FaceRecognizer*> ptr_recognizers(real_num_threads);
	for (int i = 0; i < real_num_threads; i++)
	{
		ptr_detectors[i] = &detectors[i];
		ptr_recognizers[i] = &recognizers[i];
	}
	return ZQ_FaceDatabaseMaker::CropImagesForDatabase(ptr_detectors, ptr_recognizers, src_fold, dst_fold, real_num_threads, strict_check, "err_log.txt", true);
}

int main(int argc, const char** argv)
{

	if (argc < 3)
	{
		std::cout << "Use: " << std::string(argv[0]) << " src_root dst_root [max_thread_num] [strict_check(0/1)]\n";
		return EXIT_FAILURE;
	}

	int max_thread_num = 32;
	bool strict_check = false;
	if (argc > 3)
		max_thread_num = atoi(argv[3]);
	if (argc > 4)
		strict_check = atoi(argv[4]);
	if (!CropImagesForDatabase(argv[1], argv[2], max_thread_num, strict_check))
	{
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}
#else
#include <stdio.h>
int main(int argc, const char** argv)
{
	printf("%s only support windows\n", argv[0]);
	return EXIT_SUCCESS;
}
#endif