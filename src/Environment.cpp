#include <iostream>

#include <cstdlib>
#include <ctime>

#include "WordAgent.hpp"
#include "Parameter.hpp"
#include "Evaluation.hpp"

using namespace std;

Environment::Environment(int r, int c, Evaluation * evaluation, Model *model)
		: rows(r), cols(c)
{
	resetAgents();
	eva = evaluation;
	mod = model;
	pAntigenNum = 0;
	pBcellNum = 0;
	agentId = 0;
	_initGrid();

	badrow.clear();
	badcol.clear();
}

int Environment::_calcSub(const pair<int, int> & pos) const
{
	return pos.first * cols + pos.second;
}

bool Environment::addPWordAgent(WordAgent & pWordAgent)
{
        agentId++;
        pWordAgent.setAgentID(agentId);
        if(pWordAgent.getCategory() == ANTIGEN)
        {
                pAntigenNum++;
        }
        else
        {
                pBcellNum++;
        }

        pWordAgents[_calcSub(pWordAgent.getPosition())].insert(map<int,WordAgent>::value_type(agentId,pWordAgent));

	return true;
}

bool Environment::delPWordAgent(WordAgent & pWordAgent)
{
        if(pWordAgent.getCategory() == ANTIGEN)
        {
                pAntigenNum--;
        }
        else
        {
                pBcellNum--;
        }

	pWordAgents[_calcSub(pWordAgent.getPosition())].erase(pWordAgent.getAgentID());
	return true;
}

bool Environment::resetAgents()
{
	pWordAgents.clear();
	pWordAgents.resize(rows * cols, map<int,WordAgent>());
	return true;
}

int Environment::agentCount(const pair<int, int> & pos)
{
	return pWordAgents[_calcSub(pos)].size();
}

bool Environment::getNearbyAgents(const int id, const pair<int,int> & pos,
		vector<WordAgent> & neabyAgents) const
{
	neabyAgents.clear();
	const map<int,WordAgent> & near = pWordAgents[_calcSub(pos)];
	std::map<int,WordAgent>::const_iterator it = near.begin();
	for(; it != near.end(); it++){
		if((*it).first != id){

			neabyAgents.push_back((*it).second);
		}
	}
	return true;
}

pair<int, int> Environment::getRandomPosition()
{
        srand(time(NULL));
        int row = rand() % rows;
        srand(time(NULL));
        int col = rand() % cols;

	return make_pair(row, col);
}

bool Environment::xInRange(int x)
{
	return x >= 0 && x < rows;
}

bool Environment::yInRange(int y)
{
	return y >= 0 && y < cols;
}

bool Environment::update(WordAgent * pWordAgent)
{
	if(eva->calFeedback(sen,pWordAgent,fa).first < 0)
    	{
		//pWordAgent->setStatus(DIE);
		return false;
	}

	cout<<"positive feedback...."<<endl;
	int a;
	cin>>a;

	return true;
}

std::pair<int, double> Environment::gainFeedback(WordAgent * pWordAgent)
{
		return eva->calFeedback(sen,pWordAgent,fa);
}

std::vector<std::map<int, WordAgent> > Environment::getAgents()
{
	return pWordAgents;
}

void Environment::gainSentenceInfor(const Sentence & sentence, const std::vector<int> & father)
{
	sen = sentence;
	fa  = father;
}

int Environment::getAntigenNum()
{
        return pAntigenNum;
}

int Environment::getBcellNum()
{
        return pBcellNum;
}

void Environment::_initGrid()
{
        vector<int> v;
        v.resize(COLS);
        for(size_t i = 0; i < ROWS; i++)
        {
                grid.push_back(v);
        }
}

int Environment::getLocalAgentsNum(std::pair<int,int> pos)
{
        return grid[pos.first][pos.second];
}
void Environment::setLocalAgentsNum(std::pair<int,int> pos)
{
        grid[pos.first][pos.second]++;
        if(grid[pos.first][pos.second] == MAXNUMAGENT)
        {
                badrow.push_back(pos.first);
                badcol.push_back(pos.second);
        }
}

std::vector<vector<int> > Environment::getGrid()
{
        return grid;
}

void Environment::testSub(int a)
{
        for(int i = 0; i < a; i++)
        {
                for(int j = 0; j < a; j++)
                {
                        cout<<_calcSub(make_pair(i,j))<<" ";
                }
                cout<<endl;
        }
        cout<<endl;
}

void Environment::setAntigenNum()
{
        if(pAntigenNum > 0)
        {
                this->pAntigenNum--;
        }
}

void Environment::setSentence(const Sentence & sentence)
{
        sen = sentence;
}

void Environment::setFather(const vector<int> & father)
{
        fa = father;
}

std::vector<double> Environment::getFeatureWeights()
{
        return mod->getFeatureWeight();
}

bool Environment::updateFeatureWeights(std::map<int, double> & fweight)
{
        mod->updateFeatureWeight(fweight);
        return true;
}

bool Environment::cloneAgents(WordAgent * pWordAgent)
{
        //pCloneAgents.push_back(*pWordAgent);

        return true;
}

bool Environment::addCloneAgents(int cloneNumber)
{

        return true;
}




