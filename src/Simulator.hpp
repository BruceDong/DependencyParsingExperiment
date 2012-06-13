#ifndef __SIMULATOR__
#define __SIMULATOR__

#include <vector>

#include "WordAgent.hpp"
#include "Sentence.hpp"
#include "Environment.hpp"

class Environment;
class WordAgent;

class Simulator{
private:
	Environment * env;
	int times;
	std::vector<std::map<int,WordAgent> > vWordAgents;
	int rows, cols;
	int agentId;
	int agNum;
	std::vector<double> historyAccuracy;
public:
	Simulator(Environment * environment);
	bool resetAgents();
	bool addWordAgent(WordAgent & pWordAgent);
	bool deleteWordAgent(WordAgent & pWordAgent);
	bool run(const Sentence & sen, const std::vector<int> & fa);
	bool interact(WordAgent & wa) ;
	bool select(WordAgent & wa);
	bool regulateByDelete(WordAgent & wa,int N);
	int getAgNum();

	void init();

	void initAccuracy();
	void recordAccuracy(double & acc);
	std::vector<double> getAccuracy();

	bool initLocalAgents(WordAgent * wa);

	bool initLocalAgentsFlags();

private:
        bool resetInteratObjects();
        int _calcSub(const std::pair<int, int> & pos) const;
        bool _isSame(const std::vector<int> & s, const std::vector<int> & d);
        bool _release();
        bool _removeAg();
};

#endif
