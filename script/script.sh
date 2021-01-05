# !/bin/sh

GAMES=10
BLACK=0
WHITE=0
PARALLEL_CNT=0
PARALLEL_TIME=0
SERIAL_CNT=0
SERIAL_TIME=0


for i in $(seq 1 $GAMES);
do
    make clean && make; ./mcts > result.txt

    game_parallel_cnt=`cat result.txt | grep "Parallel count:" | cut -d ":" -f2 | awk 'BEGIN{cnt=0} {cnt+=$1} END{print cnt}' `
    game_serial_cnt=`cat result.txt | grep "Serial count:" | cut -d ":" -f2 | awk 'BEGIN{cnt=0} {cnt+=$1} END{print cnt}' `

    PARALLEL_CNT=`echo $PARALLEL_CNT + $game_parallel_cnt | bc`
    SERIAL_CNT=`echo $SERIAL_CNT + $game_serial_cnt | bc`

    game_parallel_time=`cat result.txt | grep "Total time cost: " | head -n 1 |   grep -Eo '[0-9]+([.][0-9]+)?'`
    game_serial_time=`cat result.txt | grep "Total time cost: " | tail -n 1 |   grep -Eo '[0-9]+([.][0-9]+)?'`

    PARALLEL_TIME=`echo $PARALLEL_TIME + $game_parallel_time | bc`
    SERIAL_TIME=`echo $SERIAL_TIME + $game_serial_time | bc`

    winner=`cat result.txt | grep "winner:" | cut -d ":" -f2`
    if [ $winner -eq 0 ]; then
        BLACK=$((BLACK+1))
    else
        WHITE=$((WHITE+1))
    fi
done

AVG_Parallel_step=`echo $PARALLEL_CNT/$PARALLEL_TIME | bc -l`
AVG_Serial_step=`echo $SERIAL_CNT/$SERIAL_TIME | bc -l `

echo "\n\n\nIn ${GAMES} Games"
echo "The average win rate of black is ${BLACK}/${GAMES}"

echo "The total simulation count of parallel is ${PARALLEL_CNT}"
echo "The total simulation count of serial is ${SERIAL_CNT}"

echo "The total time of parallel is ${PARALLEL_TIME}"
echo "The total time of serial is ${SERIAL_TIME}"

echo "The average steps of Parallel is $AVG_Parallel_step steps/(ms)"
echo "The average steps of serial is $AVG_Serial_step steps/(ms)"


