#ifndef __WORD_AGENT__
#define __WORD_AGENT__

#include <string>
#include <map>
#include <vector>
#include <queue>
#include <set>

#include "Environment.hpp"
class Environment;

class WordAgent{
private:
        int AgentID;
	int ID;
	int AGID;
	std::pair<int, int> position;
	std::map<int, double> domFeature;
	std::map<int, double> tmpFeature;
	std::vector<int> recFeature;
	std::vector<int> agFeature;
	std::vector<int> mutatePosition;
	Environment * env;
	std::queue<int> orders;
	double domAffinity;
	double recAffinity;
	double agAffinity;
	double mutatedAffinity;
	int status;
	int category;
	double stimulus;
	double suppression;
	int concentration;
	std::pair<int, double> feedback;
	bool isInteractedWithAntigen;
	Sentence sen;
	int senID;

public:
	WordAgent(int id,
			Environment * environment,
			const std::pair<int, int> & pos, int cat, int con);
        /*running agent*/
	bool run();

	/*operation on id*/
	int     getID();
	int     getAGID();
	bool    setAGID(int id);
	int     getAgentID();
	bool    setAgentID(const int id);

	/*adding features*/
	bool    addDomFeature(const std::vector<int> & feature);
	bool    addRecFeature(const std::vector<int> & feature);

	/*getting category*/
	int getCategory();

	/*operation on status*/
	int     getStatus();
	void    setStatus(int s);

	/*getting affinity*/
	double getAgAffinity();
	double getMutatedAffinity();

	/*operation on position*/
	std::pair<int, int> getPosition() const;
	void setPosition(std::pair<int,int> p);

	/*operation on receptor*/
	std::map<int, double>   getDomReceptor();
	std::map<int, double>   getTmpReceptor();
        std::vector<int>        getRecReceptor() const;
	bool setDomReceptor(std::map<int, double> & rec);
	bool setRecReceptor(std::vector<int> & rec);

	/*operation on concentration*/
	int     getConcentration();
	void    updateConcentration();

	/*operation on sentence*/
	Sentence getSentence();
	bool setSentence(const Sentence & sentence, int sentenceID);
private:
        /*Behaviors*/
	bool _doMove();
	bool _interact();
	bool _mutate();
	bool _select();
	bool _clone();
	bool _regulate();
	bool _die();

        /*stating concentration*/
	int     _calConcentration();
	/*getting information on regulation*/
	bool    _getRegulation();
        /*updating self receptors*/
	bool    _updateSelf();
	/*calculating feedback*/
	bool    _calFeedback();
	/*mapping status to behaviors*/
	void    _mapStatusToBehavior();
	/*calculating affinity*/
	double  _calAffinity(std::vector<int> receptor, int & matchSize);
	double  _calMutatedAffinity(std::vector<int> receptor);
	double  _calSuppressByBcell(std::map<int, double> receptor);
	double  _calStimulusByBcell(std::vector<int> receptor);

};

#endif
