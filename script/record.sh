# !/bin/sh

GAMES=10
BLACK=0
WHITE=0
PLAYER_CNT=0
PLAYER_TIME=0
ENV_CNT=0
ENV_TIME=0

PROJECT_ROOT=$(pwd | rev | cut -d / -f 1 --complement | rev )
PROJECT_ROOT=$PROJECT_ROOT/build
# /home/cyctw/Documents/PP/FinalPP/Parallel-MCTS/build

cd $PROJECT_ROOT

for i in $(seq 1 $GAMES);
do
    ./mcts $@ > result.txt

    game_player_cnt=`sed -n '/Player/,/Envir/p' tesst | grep "Total search count: " | cut -d ":" -f2 | cut -d " " -f2`
    game_env_cnt=`sed -n '/Envir/,/times/p' tesst | grep "Total search count: " | cut -d ":" -f2 | cut -d " " -f2`

    PLAYER_CNT=`echo $PLAYER_CNT + $game_player_cnt | bc`
    ENV_CNT=`echo $ENV_CNT + $game_env_cnt | bc`

    game_player_time=`sed -n '/Player/,/Envir/p' result.txt | grep "Total time cost: " | cut -d ":" -f2 | cut -d " " -f2`
    game_env_time=`sed -n '/Envir/,/times/p' tesst | grep "Total time cost: " | cut -d ":" -f2 | cut -d " " -f2`

    PLAYER_TIME=`echo $PLAYER_TIME + $game_player_time | bc`
    ENV_TIME=`echo $ENV_TIME + $game_env_time | bc`

    winner=`cat result.txt | grep "winner:" | cut -d ":" -f2`
    if [ $winner = "Black" ]; then
        BLACK=$((BLACK+1))
    elif [ $winner = "White" ]; then
        WHITE=$((WHITE+1))
    else 
        BLACK=`echo $BLACK + 0.5 | bc`
        WHITE=`echo $WHITE + 0.5 | bc`
    fi
done
AVG_Player_step=`echo $PLAYER_CNT/$PLAYER_TIME | bc -l`
AVG_Env_step=`echo $ENV_CNT/$ENV_TIME | bc -l `

echo "In ${GAMES} Games"
# echo `cat result.txt | sed -n '/simulationCount:/,/+/p' | head -n -1`
echo `cat result.txt | sed '1,1!d'`
echo `cat result.txt | sed '2,2!d'`
echo `cat result.txt | sed '3,3!d'`
echo `cat result.txt | sed '4,4!d'`
echo `cat result.txt | sed '5,5!d'`
echo 
echo 
echo "The average win rate of black is ${BLACK}/${GAMES}"

echo "The total simulation count of black is ${PLAYER_CNT}"
echo "The total simulation count of white is ${ENV_CNT}"

echo "The total time of black is ${PLAYER_TIME}"
echo "The total time of white is ${ENV_TIME}"

echo "The average steps of black is $AVG_Player_step steps/(ms)"
echo "The average steps of white is $AVG_Env_step steps/(ms)"


