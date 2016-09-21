#include "GameRules.h"
#include "GlobalDefine.h"
#include "GlobalFunc.h"

#define COMPARE_GREATER(a, b) GlobalFunc::cmp_sort(a, b)
#define COMPATE_EQUAL(a, b) (!GlobalFunc::cmp_sort(a, b) && !GlobalFunc::cmp_sort(b, a))

PokerValueType GameRules::analysePokerValueType(Vector<Poker*> _pokers){
	if (_pokers.size() == 0) return NONE;
	if (_pokers.size() < 5){
		if (isSingle(_pokers) == true) return SINGLE;
		if (isPair(_pokers) == true) return PAIR;
		if (isTriple(_pokers) == true) return TRIPLE;
		if (isBomb(_pokers) == true) return BOMB;
		if (isKingBomb(_pokers) == true) return KINGBOMB;
	}
	if (isStraight(_pokers) == true) return STRAIGHT;
	if (isPairStraight(_pokers) == true) return PAIRSRAIGHT;
	if (isTripleStraight(_pokers) == true) return TRIPLESTRAIGHT;

	return NONE;
}

bool GameRules::isPokerValueType(Vector<Poker*> _pokers, PokerValueType pokerValueType){
	switch (pokerValueType)
	{
	case NONE: return false; break;
	case SINGLE: return isSingle(_pokers); break;
	case PAIR: return isPair(_pokers); break;
	case TRIPLE: return isTriple(_pokers); break;
	case BOMB: return isBomb(_pokers); break;
	case KINGBOMB: return isKingBomb(_pokers); break;
	case STRAIGHT: return isStraight(_pokers); break;
	case PAIRSRAIGHT: return isPairStraight(_pokers); break;
	case TRIPLESTRAIGHT: return isTripleStraight(_pokers); break;
	default: return false; break;
	}
	return false;
}

Vector<Poker*> GameRules::calcPokerWithValueType(Vector<Poker*> _pokers, PokerValueType pokerValueType, const Poker* _poker /* = nullptr */, int length /* = 0 */){
	switch (pokerValueType){
	case SINGLE: return searchSingle(_pokers, _poker); break;
	case PAIR: return searchPair(_pokers, _poker); break;
	case TRIPLE: return searchTriple(_pokers, _poker); break;
	case BOMB: return searchBomb(_pokers, _poker); break;
	case KINGBOMB: return searchKingBomb(_pokers); break;
	case STRAIGHT: return searchSingleStraight(_pokers, length, _poker); break;
	case PAIRSRAIGHT: return searchPairStraight(_pokers, length, _poker); break;
	case TRIPLESTRAIGHT: return searchTripleStraight(_pokers, length, _poker); break;
	default: break;
	}
	return Vector<Poker*>();
}

Vector<Poker*> GameRules::searchSingle(Vector<Poker*> _pokers, const Poker* _poker /* = nullptr */){

	if (_pokers.size() == 0) return Vector<Poker*>();

	int index = _pokers.size() - 1;	/* 因为牌面从大到小排序的，因此需要从高位来查找 */
	///* 根据所要比较的poker来查找所需的扑克，一般是大1即可 */
	//Poker* searchPoker = GlobalFunc::searchGreaterPoker(_poker, 1);
	while (index >= 0){
		Vector<Poker*> tmp;
		tmp.pushBack(_pokers.at(index));
		/* 首先判断是否牌面值大于要找的牌，如果是，那就可以进一步判断是对子，三张，炸弹等，这样可以减轻计算量 
			判断对子等的作用是防止拆牌，在非顺子的前提下，不可以拆牌*/
		if (_poker == nullptr || COMPARE_GREATER(tmp.at(0), _poker) == true){
			if (index - 1 >= 0){
				tmp.pushBack(_pokers.at(index - 1));
				if (isPair(tmp) == true){
					if (index - 2 >= 0){
						tmp.pushBack(_pokers.at(index - 2));
						if (isTriple(tmp) == true){
							if (index - 3 >= 0){
								tmp.pushBack(_pokers.at(index - 3));
								if (isBomb(tmp) == true){
									index = index - 4;
									continue;
								}
							}
							index = index - 3;
							continue;
						}
					}
					index = index - 2;
					continue;
				}
			}
			return tmp;
		}
		index = index - 1;
	}
	/* 存在的一个问题，为了不拆牌，可能出现333444555这种情况时，不会出单张 */
	return Vector<Poker*>();
}

Vector<Poker*> GameRules::searchPair(Vector<Poker*> _pokers, const Poker* _poker /* = nullptr */){
	if (_pokers.size() < 2) return Vector<Poker*>();

	int index = _pokers.size() - 1;
	/* >= 1的目的是，保证每轮循环至少有两张牌 */
	while (index >= 1){
		Vector<Poker*> tmp;
		tmp.pushBack(_pokers.at(index));
		tmp.pushBack(_pokers.at(index - 1));
		if (isPair(tmp) == true && (_poker == nullptr || COMPARE_GREATER(tmp.at(0), _poker) == true)){
			if (index - 2 >= 0){
				tmp.pushBack(_pokers.at(index - 2));
				if (isTriple(tmp) == true){
					if (index - 3 >= 0){
						tmp.pushBack(_pokers.at(index - 3));
						if (isBomb(tmp) == true){
							index = index - 4;
							continue;
						}
					}
					index = index - 3;
					continue;
				}
			}
			return tmp;
		}
		index = index - 1;
	}
	return Vector<Poker*>();
}

Vector<Poker*> GameRules::searchTriple(Vector<Poker*> _pokers, const Poker* _poker /* = nullptr */){
	if (_pokers.size() < 3) return Vector<Poker*>();

	int index = _pokers.size() - 1;
	while (index >= 2){
		Vector<Poker*> tmp;
		tmp.pushBack(_pokers.at(index));
		tmp.pushBack(_pokers.at(index - 1));
		tmp.pushBack(_pokers.at(index - 2));

		if (isTriple(tmp) == true && (_poker == nullptr || COMPARE_GREATER(tmp.at(0), _poker) == true)){
			if (index - 3 >= 0){
				tmp.pushBack(_pokers.at(index - 3));
				if (isBomb(tmp) == true){
					index = index - 4;
					continue;
				}
			}
			return tmp;
		}
		index = index - 1;
	}
	return Vector<Poker*>();
}

Vector<Poker*> GameRules::searchBomb(Vector<Poker*> _pokers, const Poker* _poker /* = nullptr */){
	if (_pokers.size() < 4) return Vector<Poker*>();

	int index = _pokers.size() - 1;
	while (index >= 3){
		Vector<Poker*> tmp;
		tmp.pushBack(_pokers.at(index));
		tmp.pushBack(_pokers.at(index - 1));
		tmp.pushBack(_pokers.at(index - 2));
		tmp.pushBack(_pokers.at(index - 3));
		if (isBomb(tmp) == true && (_poker == nullptr || COMPARE_GREATER(tmp.at(0), _poker) == true)){
			return tmp;
		}
		index = index - 1;
	}
}

/* 查找固定值的扑克 */
Vector<Poker*> GameRules::searchSpecifiedSingle(Vector<Poker*> _pokers, const Poker* _poker){
	if (_pokers.size() < 1 || _poker == nullptr) return Vector<Poker*>();
	int index = _pokers.size() - 1;
	while (index >= 0){
		Vector<Poker*> tmp;
		tmp.pushBack(_pokers.at(index));
		if (COMPATE_EQUAL(tmp.at(0), _poker) == true) return tmp;
		/* 如果当前扑克已经大于要找的扑克，那么直接break */
		if (COMPARE_GREATER(tmp.at(0), _poker) == true) break;
		index = index - 1;
	}
	return Vector<Poker*>();
}

Vector<Poker*> GameRules::searchSpecifiedPair(Vector<Poker*> _pokers, const Poker* _poker){
	if (_pokers.size() < 2 || _poker == nullptr) return Vector<Poker*>();
	int index = _pokers.size() - 1;
	while (index >= 1){
		Vector<Poker*> tmp;
		tmp.pushBack(_pokers.at(index));
		tmp.pushBack(_pokers.at(index - 1));
		if (isPair(tmp) == true){
			if (COMPATE_EQUAL(tmp.at(0), _poker) == true) return tmp;
			/* 如果当前扑克已经大于要找的扑克，那么直接break */
			if (COMPARE_GREATER(tmp.at(0), _poker) == true) break;
			index = index - 2;
		}else{
			index = index - 1;
		}
	}
	return Vector<Poker*>(); 
}

Vector<Poker*> GameRules::searchSpecifiedTriple(Vector<Poker*> _pokers, const Poker* _poker){
	if (_pokers.size() < 3 || _poker == nullptr) return Vector<Poker*>();

	int index = _pokers.size() - 1;
	while (index >= 2){
		Vector<Poker*> tmp;
		tmp.pushBack(_pokers.at(index));
		tmp.pushBack(_pokers.at(index - 1));
		tmp.pushBack(_pokers.at(index - 2));
		if (isTriple(tmp) == true){
			if (COMPATE_EQUAL(tmp.at(0), _poker) == true) return tmp;
			if (COMPARE_GREATER(tmp.at(0), _poker) == true) break; 
			index = index - 3;
		}
		index = index - 1;
	}
	return Vector<Poker*>();
}

/* 该函数暂时不用，待后面找到优化和合并的办法再写 */
//Vector<Poker*> GameRules::searchStraight(Vector<Poker*> _pokers, int step, int length, int value /* = 0 */){
//	CCASSERT(value < 0, "该函数不可调用");
//	return Vector<Poker*>();
//}

Vector<Poker*> GameRules::searchSingleStraight(Vector<Poker*> _pokers, int length, const Poker* _poker){
	/* 如果length参数出错，或者扑克的张数不足所需的length，那么直接返回空 */
	if (length < 5 || length > 12 || _pokers.size() < length) return Vector<Poker*>();
	
	/* 如果查找的顺子的最小的牌面值是1或者2，以及所需顺子牌面最低值+顺子长度（此时就是顺子牌面的最大值）> A（14）,那么直接返回空 */
	int pokerValue = _poker->getValue();
	/* 如果查找的牌值是0，表明是大王或者小王 */
	if (pokerValue == 0 || pokerValue == 1 || pokerValue == 2 || (pokerValue + length > 14)) return Vector<Poker*>();
	
	int index = pokerValue + 1;
	Vector<Poker*> ret;
	while (index <= (14 - length + 1)){
		for (int i = 0; i < length; ++i, ++index){
			auto tmpPoker = GlobalFunc::getGreaterPoker(_poker, index - pokerValue);
			Vector<Poker*> tmp = searchSpecifiedSingle(_pokers, tmpPoker);
			if (tmp.size() != 0){
				ret.pushBack(tmp.at(0));
			}else{
				ret.clear();
				break;
			}
		}
		if (ret.size() == length) return ret;
		index++; /* 当前已经查找失败，将index+1，从下一个开始查找 */
	}
	return Vector<Poker*>();
}

/* length这里是顺子的不同牌的个数 */
Vector<Poker*> GameRules::searchPairStraight(Vector<Poker*> _pokers, int length, const Poker* _poker){
	if (length < 3 || length > 12 || _pokers.size() < length * 2) return Vector<Poker*>();
	int pokerValue = _poker->getValue();
	if (pokerValue == 0 || pokerValue == 1 || pokerValue == 2 || (pokerValue + length > 14)) return Vector<Poker*>();

	int index = pokerValue + 1;
	Vector<Poker*> ret;
	while (index <= (14 - length + 1)){
		for (int i = 0; i < length; ++i, ++index){
			auto tmpPoker = GlobalFunc::getGreaterPoker(_poker, index - pokerValue);
			Vector<Poker*> tmp = searchSpecifiedPair(_pokers, tmpPoker);
			if (tmp.size() != 0){
				ret.pushBack(tmp.at(0));
				ret.pushBack(tmp.at(1));
			} else{
				ret.clear();
				break;
			}
		}
		if (ret.size() == length * 2) return ret;
		index++; /* 当前已经查找失败，将index+1，从下一个开始查找 */
	}
	return Vector<Poker*>();
}

/* length这里是顺子的不同牌的个数 */
Vector<Poker*> GameRules::searchTripleStraight(Vector<Poker*> _pokers, int length, const Poker* _poker){
	if (length < 3 || length > 12 || _pokers.size() < length * 3) return Vector<Poker*>();
	int pokerValue = _poker->getValue();
	if (pokerValue == 0 || pokerValue == 1 || pokerValue == 2 || (pokerValue + length > 14)) return Vector<Poker*>();

	int index = pokerValue + 1;
	Vector<Poker*> ret;
	while (index <= (14 - length + 1)){
		for (int i = 0; i < length; ++i, ++index){
			auto tmpPoker = GlobalFunc::getGreaterPoker(_poker, index - pokerValue);
			Vector<Poker*> tmp = searchSpecifiedPair(_pokers, tmpPoker);
			if (tmp.size() != 0){
				ret.pushBack(tmp.at(0));
				ret.pushBack(tmp.at(1));
				ret.pushBack(tmp.at(2));
			}
			else{
				ret.clear();
				break;
			}
		}
		if (ret.size() == length * 3) return ret;
		index++; /* 当前已经查找失败，将index+1，从下一个开始查找 */
	}
	return Vector<Poker*>();
}

bool GameRules::isSingle(Vector<Poker*> _pokers){
	return _pokers.size() == 1;
}

bool GameRules::isPair(Vector<Poker*> _pokers){
	if (_pokers.size() != 2) return false;
	return _pokers.at(0) == _pokers.at(1);
}

bool GameRules::isTriple(Vector<Poker*> _pokers){
	if (_pokers.size() != 3) return false;
	return (_pokers.at(0)->getValue() == _pokers.at(1)->getValue()) 
		&& (_pokers.at(0)->getValue() == _pokers.at(2)->getValue());
}

bool GameRules::isBomb(Vector<Poker*> _pokers){
	if (_pokers.size() != 4) return false;
	return(_pokers.at(0)->getValue() == _pokers.at(1)->getValue())
		&& (_pokers.at(0)->getValue() == _pokers.at(2)->getValue())
		&& (_pokers.at(0)->getValue() == _pokers.at(3)->getValue());
}

bool GameRules::isKingBomb(Vector<Poker*> _pokers){
	if (_pokers.size() != 2) return false;
	PokerType type_1 = _pokers.at(0)->getPokerType();
	PokerType type_2 = _pokers.at(1)->getPokerType();
	return (type_1 == REDJOKER || type_1 == BLACKJOKER)
		&& (type_2 == REDJOKER || type_2 == BLACKJOKER);
}

bool GameRules::isStraight(Vector<Poker*> _pokers, int step /* = 1 */){
	GlobalFunc::sort(_pokers);	/* 对扑克进行排序 */

	auto _first_poker = _pokers.at(0);
	/* 如果第一张是王，或者2，那么肯定不是顺子 */
	if (_first_poker->getValue() == 2
		|| _first_poker->getPokerType() == BLACKJOKER
		|| _first_poker->getPokerType() == REDJOKER){
		return false;
	}

	auto secondpoker_index = 0 + step;	/* 第二张牌的索引，这里的第二张是不同牌值的第二张，例如AAKK，那么第二张是K */

	if (_first_poker->getValue() == 1){
		if (_pokers.at(secondpoker_index)->getValue() != 13){
			return false;
		}else{
			for (int i = secondpoker_index; i < _pokers.size() - step; i += step){
				if (_pokers.at(i)->getValue() != (_pokers.at(i + step)->getValue() + 1)){
					return false;
				}
			}
			return true;
		}
	}else{
		for (int i = 0; i < _pokers.size() - step; i += step){
			if (_pokers.at(i)->getValue() != (_pokers.at(i + step)->getValue() + 1)){
				return false;
			}
		}
		return true;
	}
}

bool GameRules::isPairStraight(Vector<Poker*> _pokers){
	return isStraight(_pokers, 2);
}

bool GameRules::isTripleStraight(Vector<Poker*> _pokers){
	return isStraight(_pokers, 3);
}