pipeline
{
    agent any
    stages
    {
        stage('build')
        {
            steps
            {
                sh 'make -j\$(nproc)'
            }
        }
        stage('check')
        {
            steps
            {
                cppcheck("src")
            }
        }
    }
    post
    {
        always
        {
            deleteDir() /* clean up our workspace */
        }
        success
        {
            slackstatus("success")
        }
        failure
        {
            slackstatus("fail")
        }
        unstable
        {
            slackstatus("unstable")
        }

    }
}
