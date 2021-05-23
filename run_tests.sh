#! /bin/sh

testFiles=$(ls ./tests/*.um)

for testFile in $testFiles ; do
    testName=$(echo $testFile | sed -E 's/(.*).um/\1/')
    testName="${testName:8}"
    echo $testName
    
    umdump $testFile >$2testing/dumps/$testName.dump
    
    input='/dev/stdin'
    output='/dev/stdout'
    compare='/dev/null'
    error=./testing/errors/$testName.error
    
    if [ -f ./testing/tests/$testName.1 ] ; then
        output=./output/$testName.out
        compare=./tests/$testName.1
    fi

    if [ -f ./testing/tests/$testName.0 ] ; then
        input=./tests/$testName.0
    fi 

    { ./um $testFile <$input >$output ;} 2>$error

    if [ $output != '/dev/stdout' ] ; then
        result=$(diff $output $compare)

        if [ "$result" == "" ] ; then 
            echo '  * test passed'
        else
            echo '  * test failed'
        fi 
    fi

    words=$(wc -c $error)

    if [ "${words}" != "0 ${error}" ] ; then 
        echo '  * test errored successfully'
    fi


done 

