#include "commons.h"
#include "Imp.h"
#include <math.h>

using namespace game_utils;

namespace game_objects
{
	CImp::CImp(): CCreature()
	{
		impState = IS_IDLE;
		moveSpeed = 0.0005f;
	}

	CImp::~CImp()
	{
	}

	GLvoid CImp::checkNearestForDigging()
	{
		if(impState != IS_IDLE) return;

		std::vector<CBlock*> markedBlocks,possBlocks;
		std::vector<CBlock*>::iterator markedBlocksIter;
		CBlock *block;

		if(CV_GAME_MANAGER->getLevelManager()->isBlockTypeNear(CV_BLOCK_TYPE_EARTH_ID,cml::vector2i((int)floor(position[0]/CV_BLOCK_WIDTH),(int)floor(position[2]/CV_BLOCK_DEPTH)),true,CV_PLAYER_UNDEFINED,&markedBlocks))
		{
			int oldSearchLimit = CV_GAME_MANAGER->getPathManager()->getSearchLimit();
			CV_GAME_MANAGER->getPathManager()->setSearchLimit(2);
			for(markedBlocksIter=markedBlocks.begin(); markedBlocksIter!=markedBlocks.end(); markedBlocksIter++)
			{
				block = *markedBlocksIter;
				markedBlocks.clear();
				path.clear();
					if(CV_GAME_MANAGER->getPathManager()->findPath(cml::vector2i((int)floor(position[0]/CV_BLOCK_WIDTH),(int)floor(position[2]/CV_BLOCK_DEPTH)),block->getLogicalPosition(),&path))
						possBlocks.push_back(block);
			}
			CV_GAME_MANAGER->getPathManager()->setSearchLimit(oldSearchLimit);
			if(possBlocks.size()>0)
			{
				GLint blockNum = rand()%possBlocks.size();
				path.clear();
				path.push_back(possBlocks[blockNum]->getLogicalPosition());
				impState = IS_GOING_TO_DIGGING_DESTINATION;
				return;
			}
		}
	}

	GLvoid CImp::checkNearestForClaiming()
	{
		if(impState != IS_IDLE) return;

		std::vector<CBlock*> unclaimedBlocks,claimedBlocks,possBlocks;
		std::vector<CBlock*>::iterator unclaimedBlocksIter, claimedBlocksIter;
		CBlock *block;

		if(CV_GAME_MANAGER->getLevelManager()->isBlockTypeNear(CV_BLOCK_TYPE_UNCLAIMED_LAND_ID,cml::vector2i((int)floor(position[0]/CV_BLOCK_WIDTH),(int)floor(position[2]/CV_BLOCK_DEPTH)),true,CV_PLAYER_UNDEFINED,&unclaimedBlocks))
		{
			int oldSearchLimit = CV_GAME_MANAGER->getPathManager()->getSearchLimit();
			CV_GAME_MANAGER->getPathManager()->setSearchLimit(2);
			for(unclaimedBlocksIter=unclaimedBlocks.begin(); unclaimedBlocksIter!=unclaimedBlocks.end(); unclaimedBlocksIter++)
			{
				block = *unclaimedBlocksIter;
				if(block->isTaken())
					continue;
				claimedBlocks.clear();
				if(CV_GAME_MANAGER->getLevelManager()->isBlockTypeNear(CV_BLOCK_TYPE_CLAIMED_LAND_ID,block->getLogicalPosition(),false,CV_CURRENT_PLAYER_ID,&claimedBlocks))
				{
					path.clear();
					if(CV_GAME_MANAGER->getPathManager()->findPath(cml::vector2i((int)floor(position[0]/CV_BLOCK_WIDTH),(int)floor(position[2]/CV_BLOCK_DEPTH)),block->getLogicalPosition(),&path))
						possBlocks.push_back(block);
				}
			}
			CV_GAME_MANAGER->getPathManager()->setSearchLimit(oldSearchLimit);
			if(possBlocks.size()>0)
			{
				GLint blockNum = rand()%possBlocks.size();
				path.clear();
				path.push_back(possBlocks[blockNum]->getLogicalPosition());
				possBlocks[blockNum]->setTaken(true);
				impState = IS_GOING_TO_CLAIMING_DESTINATION;
				return;
			}
		}
	}

	GLvoid CImp::checkNearestForWalling()
	{
		if(impState != IS_IDLE) return;
	}

	GLvoid CImp::checkForDigging()
	{
		if(impState != IS_IDLE) return;
		path.clear();
		CBlock *block = CV_GAME_MANAGER->getLevelManager()->getMarkedBlock(CV_CURRENT_PLAYER_ID);
		if (block)
		{
			cml::vector2i currPos = cml::vector2i((int)floor(position[0]/CV_BLOCK_WIDTH),(int)floor(position[2]/CV_BLOCK_DEPTH));
			if(CV_GAME_MANAGER->getPathManager()->findPath(currPos,block->getLogicalPosition(),&path))
				impState = IS_GOING_TO_DIGGING_DESTINATION;
		}
	}

	GLvoid CImp::checkForClaiming()
	{
		if(impState != IS_IDLE) return;
		path.clear();
		CBlock *block = CV_GAME_MANAGER->getLevelManager()->getUnclaimedBlock(CV_CURRENT_PLAYER_ID);
		if (block)
		{
			cml::vector2i currPos = cml::vector2i((int)floor(position[0]/CV_BLOCK_WIDTH),(int)floor(position[2]/CV_BLOCK_DEPTH));
			if(CV_GAME_MANAGER->getPathManager()->findPath(currPos,block->getLogicalPosition(),&path))
				impState = IS_GOING_TO_CLAIMING_DESTINATION;
			else
				block->setTaken(false);
		}
	}

	GLvoid CImp::checkForWalling()
	{
		if(impState != IS_IDLE) return;
	}

	GLvoid CImp::walkPath(GLfloat deltaTime)
	{
		if(path.size() == 0)
		{
			if(impState == IS_GOING_TO_CLAIMING_DESTINATION)
				impState = IS_AT_CLAIMING_BLOCK;
			else if (impState == IS_GOING_TO_DIGGING_DESTINATION)
				impState = IS_AT_DIGGING_BLOCK;
			else if (impState == IS_GOING_TO_WALLING_DESTINATION)
				impState = IS_AT_WALLING_BLOCK;
			return;
		}
		cml::vector2i point = path.back();
		GLfloat tX = (GLfloat)point[0]*CV_BLOCK_WIDTH+CV_BLOCK_WIDTH/2.0f;
		GLfloat tZ = (GLfloat)point[1]*CV_BLOCK_DEPTH+CV_BLOCK_DEPTH/2.0f;

		if (fabs(position[0]-tX)<=0.01f && fabs(position[2]-tZ)<=0.01f)
		{
			path.pop_back();
			if(path.size()==0)
			{
				moveVector[0] = 0.0f;
				moveVector[2] = 0.0f;
				return;
			}
			point = path.back();
			tX = (GLfloat)point[0]*CV_BLOCK_WIDTH+CV_BLOCK_WIDTH/2.0f;
			tZ = (GLfloat)point[1]*CV_BLOCK_DEPTH+CV_BLOCK_DEPTH/2.0f;
		}
		
		//calculate new movement direction
		moveVector[0] = tX-position[0];
		moveVector[2] = tZ-position[2];
		moveVector.normalize();
		
		rotation[1] = 90.0f-(float)(atan2(moveVector[2],moveVector[0])*180.0f/M_PI);

		cml::vector3f oldPos = position;
		position += moveVector*moveSpeed*deltaTime;

		if ((position[0]-tX > 0.0f && oldPos[0]-tX<0.0f)
			|| (position[0]-tX < 0.0f && oldPos[0]-tX>0.0f))
			position[0] = tX;
		if ((position[2]-tZ > 0.0f && oldPos[2]-tZ<0.0f)
			|| (position[2]-tZ < 0.0f && oldPos[2]-tZ>0.0f))
			position[2] = tZ;
			
	}

	GLvoid CImp::update(GLfloat deltaTime)
	{
		if (impState == IS_IDLE)
		{
			//check for next space digging
			//check for next space claiming
			//check for next space walling

			//check for digging
			//check for claiming
			//check for walling

			checkNearestForDigging();
			checkNearestForClaiming();
			checkNearestForWalling();

			checkForDigging();
			checkForClaiming();
			checkForWalling();
		} else if (impState == IS_GOING_TO_DIGGING_DESTINATION)
		{
			walkPath(deltaTime);
		} else if (impState == IS_GOING_TO_CLAIMING_DESTINATION)
		{
			walkPath(deltaTime);
		} else if (impState == IS_GOING_TO_WALLING_DESTINATION)
		{
			walkPath(deltaTime);
		} else if (impState == IS_AT_DIGGING_BLOCK)
		{
			impState = IS_DIGGING;
			useAction(AA_CLAIM);
		} else if (impState == IS_AT_CLAIMING_BLOCK)
		{
			impState = IS_CLAIMING;
			useAction(AA_CLAIM);
		} else if (impState == IS_AT_WALLING_BLOCK)
		{
			impState = IS_WALLING;
			useAction(AA_CLAIM);
		} else if (impState == IS_DIGGING)
		{
		CV_GAME_MANAGER->getLevelManager()->getBlock(cml::vector2i((int)floor(position[0]/CV_BLOCK_WIDTH),(int)floor(position[2]/CV_BLOCK_DEPTH)))->decLife(deltaTime*moveSpeed);
		if (CV_GAME_MANAGER->getLevelManager()->getBlock(cml::vector2i((int)floor(position[0]/CV_BLOCK_WIDTH),(int)floor(position[2]/CV_BLOCK_DEPTH)))->getLife()<=0.0f)
		{
			CV_GAME_MANAGER->getLevelManager()->getBlock(cml::vector2i((int)floor(position[0]/CV_BLOCK_WIDTH),(int)floor(position[2]/CV_BLOCK_DEPTH)))->digBlock();

			impState = IS_IDLE;
			useAction(AA_WALK);
		}
		} else if (impState == IS_CLAIMING)
		{
			CV_GAME_MANAGER->getLevelManager()->getBlock(cml::vector2i((int)floor(position[0]/CV_BLOCK_WIDTH),(int)floor(position[2]/CV_BLOCK_DEPTH)))->decLife(deltaTime*moveSpeed);
			if (CV_GAME_MANAGER->getLevelManager()->getBlock(cml::vector2i((int)floor(position[0]/CV_BLOCK_WIDTH),(int)floor(position[2]/CV_BLOCK_DEPTH)))->getLife()<=0.0f)
			{
				CV_GAME_MANAGER->getLevelManager()->getBlock(cml::vector2i((int)floor(position[0]/CV_BLOCK_WIDTH),(int)floor(position[2]/CV_BLOCK_DEPTH)))->claimBlock(CV_CURRENT_PLAYER_ID);
				impState = IS_IDLE;
				useAction(AA_WALK);
			}
		} else if (impState == IS_WALLING)
		{
			CV_GAME_MANAGER->getLevelManager()->getBlock(cml::vector2i((int)floor(position[0]/CV_BLOCK_WIDTH),(int)floor(position[2]/CV_BLOCK_DEPTH)))->decLife(deltaTime*moveSpeed);
			if (CV_GAME_MANAGER->getLevelManager()->getBlock(cml::vector2i((int)floor(position[0]/CV_BLOCK_WIDTH),(int)floor(position[2]/CV_BLOCK_DEPTH)))->getLife()<=0.0f)
			{
				CV_GAME_MANAGER->getLevelManager()->getBlock(cml::vector2i((int)floor(position[0]/CV_BLOCK_WIDTH),(int)floor(position[2]/CV_BLOCK_DEPTH)))->fortifyBlock(CV_CURRENT_PLAYER_ID);
				impState = IS_IDLE;
				useAction(AA_WALK);
			}
		}
	}
};