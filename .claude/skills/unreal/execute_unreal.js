const { RemoteExecution } = require('unreal-remote-execution');
const fs = require('fs');

async function main() {
    const remoteExecution = new RemoteExecution();

    try {
        let pythonCode;

        if (process.argv.length > 2) {
            const scriptPath = process.argv[2];
            pythonCode = fs.readFileSync(scriptPath, 'utf8');
        } else {
            console.error('Usage: node execute_unreal.js <python_script_path>');
            process.exit(1);
        }

        remoteExecution.start();
        console.log('Searching for Unreal Engine instance...');

        const node = await remoteExecution.getFirstRemoteNode(1000, 10000);

        console.log('Found Unreal Engine: ' + node.data.node_name);
        console.log('Project: ' + node.data.project_name);
        console.log('Engine Version: ' + node.data.engine_version);

        await remoteExecution.openCommandConnection(node);
        console.log('Connection established\n');

        const result = await remoteExecution.runCommand(pythonCode);

        if (result.output) {
            console.log('Output:', result.output);
        }
        if (result.result) {
            console.log('Result:', result.result);
        }

        remoteExecution.stop();

        if (!result.success) {
            process.exit(1);
        }
    } catch (error) {
        console.error('Error:', error.message);
        remoteExecution.stop();
        process.exit(1);
    }
}

main();
