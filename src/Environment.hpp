#ifndef __ENVIRONMENT__
#define __ENVIRONMENT__

#include <vector>
#include <map>
#include <set>

#include "Sentence.hpp"
#include "Predictor.hpp"
#include "Evaluation.hpp"
class WordAgent;
class Information;
class Evaluation;

class Environment{
private:
	std::vector<std::map<int,WordAgent> > pWordAgents;
	int rows, cols;
	Information * infor;
	Sentence sen;
	std::vector<int> fa;
	Evaluation  * eva;
	Model * mod;
	int pAntigenNum;
	int pBcellNum;
	std::vector<std::vector<int> > grid;
	std::map<int,WordAgent> pCloneAgents;

        std::vector<WordAgent *> address;
        std::vector<WordAgent *> delAddress;
	std::vector<int> badrow;
	std::vector<int> badcol;

	int agentId;
public:
	Environment(int r, int c, Evaluation * evaluation, Model *model);
	bool addPWordAgent(WordAgent & pWordAgent);
	bool delPWordAgent(WordAgent & pWordAgent);
	// calculation is directed
	bool resetAgents();
	bool getNearbyAgents(const int id,const std::pair<int,int> & pos,
		std::vector<WordAgent> & neabyAgents) const;
	std::vector<std::map<int, WordAgent> > getAgents();
	int agentCount(const std::pair<int, int> & pos);
	std::pair<int, int> getRandomPosition();
	bool xInRange(int x);
	bool yInRange(int y);
	bool update(WordAgent * pWordAgent);
	std::map<int, double> getInfor(WordAgent * pWordAgent);
	std::pair<int, double> gainFeedback(WordAgent * pWordAgent);
	void gainSentenceInfor(const Sentence & sentence,const  std::vector<int> & father);
	int getAntigenNum();
	void setAntigenNum();
	int getBcellNum();
	int getLocalAgentsNum(std::pair<int,int> pos);
	void setLocalAgentsNum(std::pair<int,int> pos);
	std::vector<std::vector<int> > getGrid();
	void testSub(int a);
	void setSentence(const Sentence & sentence);
	void setFather(const std::vector<int> & father);

	std::vector<double> getFeatureWeights();

	bool updateFeatureWeights(std::map<int, double> & fweight);
	bool cloneAgents(WordAgent * pWordAgent);
	bool addCloneAgents(int cloneNumber);
        bool printClone();

private:
	int _calcSub(const std::pair<int, int> & pos) const;
	void _initGrid();



};



#endif
