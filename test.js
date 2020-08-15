const test = require('ava');
const { execFile } = require('child_process');
const { getProcessList } = require('./index')

test('getProcessList', async t => {
    const child = execFile('zsh', {
        stdio: ['pipe', null, null],
    })
    child.stdin.write(`/bin/sleep 1`)
    child.stdin.end()
    await new Promise(x => setTimeout(x, 250))
    try {
        let processes = await getProcessList()
        zsh = processes.find(x => x.name === 'zsh' && x.pid === child.pid)
        t.true(!!zsh)
        t.is(zsh.children.length, 1)
        t.is(zsh.children[0].path, '/bin/sleep')
        await new Promise(x => setTimeout(x, 2000))
        processes = await getProcessList()
        t.false(processes.some(x => x.name === 'zsh' && x.pid === child.pid))
    } finally {
        child.kill()
    }
});
