QEMU=/opt/qemu-z80/bin/qemu-system-z80 
QEMU_OPTS="-s -S -nographic"
QEMU_GDB_PORT="1234"
MONITOR_GDB_PORT="9000"

GDB=~/gdb/gdb/gdb
GDB_TEST_SCRIPT=loop_test_stepi.cmd
GDB_OPTS="--nx    \
          --nw    \
          --batch \
          -l 5"
GDB_QEMU_TARGET_CMD="target extended-remote localhost:$QEMU_GDB_PORT"

echo "starting QEMU"
$QEMU $QEMU_OPTS & > /dev/null
QEMU_PID=$!
echo "pid: $QEMU_PID"

GDB_QEMU_TEST_LOG=`mktemp gdb-qemu-only.XXXX.log`
GDB_MONITOR_TEST_LOG=`mktemp gdb-monitor.XXXX.log`
echo "gdb (QEMU only) test output: $GDB_QEMU_TEST_LOG"

# rm -f output.log
$GDB $GDB_OPTS -ex "$GDB_QEMU_TARGET_CMD" --command=$GDB_TEST_SCRIPT > $GDB_QEMU_TEST_LOG

if [ $? -ne 0 ]; then
    echo "gdb terminated with error"
else
    cp opcode.log $GDB_QEMU_TEST_LOG
    rm opcode.log
    echo "gdb output: $GDB_QEMU_TEST_LOG"
fi

echo "killing QEMU ($QEMU_PID)"
kill -9 $QEMU_PID



# now test gdb against the monitor running on QEMU
echo "starting $PIPE_TO_TCP"

rm -rf /tmp/qemu_z80_wr
rm -rf /tmp/qemu_z80_rd
mkfifo /tmp/qemu_z80_wr
mkfifo /tmp/qemu_z80_rd
#$PIPE_TO_TCP &
nc -l -p $MONITOR_GDB_PORT < /tmp/qemu_z80_wr > /tmp/qemu_z80_rd &
PTCP_PID=$!
echo "pid: $PTCP_PID"

echo "starting QEMU (monitor)"
QEMU_OPTS="-nographic" # IMPORTANT: no -S or -s flags, we want QEMU to start executing the monitor code right away
$QEMU $QEMU_OPTS & > /dev/null
QEMU_PID=$!
echo "pid: $QEMU_PID"

GDB_MONITOR_TARGET_CMD="target extended-remote localhost:$MONITOR_GDB_PORT"
$GDB $GDB_OPTS -ex "$GDB_MONITOR_TARGET_CMD" --command=$GDB_TEST_SCRIPT > $GDB_MONITOR_TEST_LOG
cp opcode.log $GDB_MONITOR_TEST_LOG
rm opcode.log

echo "killing QEMU ($QEMU_PID)"
kill -9 $QEMU_PID

echo "killing $PIPE_TO_TCP ($PTCP_PID)"
kill -9 $PTCP_PID

