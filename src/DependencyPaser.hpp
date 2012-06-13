#ifndef __DEPENDENCY_PASER__
#define __DEPENDENCY_PASER__

#include <string>
#include <map>

#include "Trainer.hpp"
#include "Predictor.hpp"
#include "Model.hpp"
#include "Sentence.hpp"

class DependencyPaser{
private:
	Model * pModel;
	Trainer * pTrainer;
	Predictor * pPredictor;
	Evaluation * pEvaluation;
public:
	DependencyPaser();
	~DependencyPaser();
	bool loadModel(const char * file);
	bool saveModel(const char * file);
	bool trainFile(const char * file);
	double predict(const Sentence & sen, std::vector<int> & fa);
	bool predictFile(const char * testFile, const char * outFile);
	double evaluate(const char * resultFile, const char * evaluateFile);

	void parsing(const char * trainFile,const char * testFile, const char * outFile, const char * evaluateFile);
private:
	bool _readFileAddBCell(const char * file);
	bool _readFileTrain(const char * trainFile,const char * testFile, const char * outFile, const char * evaluateFile);
	bool _readFileTrain(const char * file);

	void _printEvaluateLine(const char * evaluateFile);
};

#endif
