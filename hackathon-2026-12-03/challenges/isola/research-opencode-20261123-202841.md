<!-- opencode model: hy4-flash-free, 2026-11-23T20:36:05+03:00 -->

> build · hy4-flash-free
I'll start by researching this CodinGame game. Let me load the relevant skill first, then begin systematic research.
→ Skill "cg-research-opencode" 
> build · hy4-flash-free
I'll research this systematically. Let me start with parallel web searches to locate the referee and public resources.
✗ Web Search via OpenCode "CodinGame Isola contest post-mortem strategy" failed
Error: Web search request failed (HTTP 402)
✗ Web Search via OpenCode "CodinGame Isola bot programming game rules referee" failed
Error: Web search request failed (HTTP 402)
✗ Web Search via OpenCode "github CodinGame Isola referee rules" failed
Error: Web search request failed (HTTP 402)
✗ Web Search via OpenCode "eulerscheZahl RefereeCollection isola" failed
Error: Web search request failed (HTTP 402)
> build · hy4-flash-free
Web search is unavailable (HTTP 402). I'll use the GitHub API and direct webfetch instead.
✗ WebFetch https://api.github.com/search/code?q=isola+repo:CodinGame/RefereeCollection failed
Error: StatusCode: non 2xx status code (401 GET https://api.github.com/search/code?q=isola+repo:CodinGame/RefereeCollection)
% WebFetch https://api.github.com/search/repositories?q=isola+codingame&per_page=20 
% WebFetch https://api.github.com/search/repositories?q=codingame+isola+in:name,description&per_page=20 
> build · hy4-flash-free
Found a referee repo. Let me inspect it and look for the official CodinGame game page.
% WebFetch https://api.github.com/repos/eulerscheZahl/RefereeCollection/contents/ 
% WebFetch https://api.github.com/repos/wala-fr/CodingameIsola/git/trees/master?recursive=1 
% WebFetch https://www.codingame.com/multiplayer/bot-programming/isola 
> build · hy4-flash-free
Excellent — found what appears to be the official game package (includes Boss, statement, levels). Let me read the referee source and constants directly.
% WebFetch https://raw.githubusercontent.com/wala-fr/CodingameIsola/master/CodingameIsolation/src/main/java/com/codingame/model/Point.java 
% WebFetch https://raw.githubusercontent.com/wala-fr/CodingameIsola/master/CodingameIsolation/src/main/java/com/codingame/parameter/Constant.java 
% WebFetch https://raw.githubusercontent.com/wala-fr/CodingameIsola/master/CodingameIsolation/src/main/java/com/codingame/game/Referee.java 
% WebFetch https://raw.githubusercontent.com/wala-fr/CodingameIsola/master/CodingameIsolation/src/main/java/com/codingame/model/Board.java 
> build · hy4-flash-free
I have the full referee. Now let me read the Boss, statement, config, and stub files.
% WebFetch https://raw.githubusercontent.com/wala-fr/CodingameIsola/master/CodingameIsolation/config/statement_en.html 
% WebFetch https://raw.githubusercontent.com/wala-fr/CodingameIsola/master/CodingameIsolation/config/Boss.java 
% WebFetch https://raw.githubusercontent.com/wala-fr/CodingameIsola/master/CodingameIsolation/config/config.ini 
% WebFetch https://raw.githubusercontent.com/wala-fr/CodingameIsola/master/CodingameIsolation/config/stub.txt 
% WebFetch https://raw.githubusercontent.com/wala-fr/CodingameIsola/master/CodingameIsolation/src/main/java/com/codingame/game/Player.java 
% WebFetch https://raw.githubusercontent.com/wala-fr/CodingameIsola/master/README.md 
> build · hy4-flash-free
The official referee is confirmed (the statement itself links to this repo). Now let me search for public bots, forum discussions, and attempt web searches via alternate engines.
% WebFetch https://www.codingame.com/forum/search?q=isola 
% WebFetch https://api.github.com/search/repositories?q=isola+codingame+bot&per_page=50 
% WebFetch https://api.github.com/search/repositories?q=codingame+isolation+game&per_page=50 
% WebFetch https://duckduckgo.com/html/?q=codingame+isola+strategy+bot 
> build · hy4-flash-free
DuckDuckGo works as a search backend. Let me run several targeted searches in parallel.
% WebFetch https://duckduckgo.com/html/?q=site%3Agithub.com+codingame+isola+bot 
% WebFetch https://duckduckgo.com/html/?q=codingame+isola+post+mortem 
% WebFetch https://duckduckgo.com/html/?q=%22CodingameIsola%22+bot+solution 
% WebFetch https://duckduckgo.com/html/?q=site%3Acodingame.com%2Fforum+isola 
% WebFetch https://duckduckgo.com/html/?q=isola+game+AI+minimax+alpha-beta+arxiv 
> build · hy4-flash-free
DDG rate-limited after a few queries, but it surfaced a key AI repo. Let me dig into it and try alternate search engines.
% WebFetch https://www.bing.com/search?q=codingame+isola+strategy+bot 
% WebFetch https://api.github.com/repos/on2valhalla/Isola/git/trees/master?recursive=1 
% WebFetch https://www.mojeek.com/search?q=codingame+isola+post+mortem 
✗ WebFetch https://api.github.com/repos/GACanova/CodinGame-Bot-Programming/git/trees/master?recursive=1 failed
Error: StatusCode: non 2xx status code (404 GET https://api.github.com/repos/GACanova/CodinGame-Bot-Programming/git/trees/master?recursive=1)
✗ WebFetch https://grep.app/api/search?q=opponentLastRemovedTile failed
Error: StatusCode: non 2xx status code (429 GET https://grep.app/api/search?q=opponentLastRemovedTile)
> build · hy4-flash-free
Let me read the Isola AI source and search academic databases and more GitHub repos.
% WebFetch https://raw.githubusercontent.com/on2valhalla/Isola/master/README.txt 
% WebFetch https://api.github.com/search/repositories?q=isola+in:name&sort=stars&per_page=50 
✗ WebFetch https://api.semanticscholar.org/graph/v1/paper/search?query=Isolation%20board%20game%20artificial%20intelligence%20heuristic&fields=title,abstract,url,year&limit=20 failed
Error: StatusCode: non 2xx status code (429 GET https://api.semanticscholar.org/graph/v1/paper/search?query=Isolation%20board%20game%20artificial%20intelligence%20heuristic&fields=title,abstract,url,year&limit=20)
✗ WebFetch http://export.arxiv.org/api/query?search_query=all:%22isolation%22+AND+all:%22game%22+AND+all:%22minimax%22&start=0&max_results=20 failed
Error: Transport error (GET http://export.arxiv.org/api/query?search_query=all:%22isolation%22+AND+all:%22game%22+AND+all:%22minimax%22&start=0&max_results=20)
✗ WebFetch https://r.jina.ai/https://www.google.com/search?q=%22codingame%22+%22isola%22+forum+strategy failed
Error: Transport error (GET https://r.jina.ai/https://www.google.com/search?q=%22codingame%22+%22isola%22+forum+strategy)
> build · hy4-flash-free
Let me try alternate search endpoints and academic APIs via scripted HTTP requests.
⚙ execute {"code":"const urls = [\n  \"https://www.bing.com/search?q=site%3Acodingame.com%2Fforum+isola&format=rss\",\n  \"https://www.bing.com/search?q=%22codingame%22+%22isola%22+bot+github&format=rss\",\n  \"https://api.semanticscholar.org/graph/v1/paper/search?query=Isolating%20game%20AI&fields=title,year,url&limit=5\",\n  \"https://export.arxiv.org/api/query?search_query=all%3Aisolation%20AND%20all%3Agame&start=0&max_results=10\",\n  \"https://grep.app/api/search?q=opponentLastRemovedTile\"\n];\nconst out = await Promise.allSettled(urls.map(async u => {\n  const r = await fetch(u, {headers:{\"User-Agent\":\"Mozilla/5.0 (research)\"}});\n  const t = await r.text();\n  return {u, status:r.status, len:t.length, head:t.slice(0,2500)};\n}));\nreturn out.map((x,i)=>x.status===\"fulfilled\"?x.value:{url:urls[i],error:String(x.reason)});"} 
> build · hy4-flash-free
⚙ execute {"code":"const grepQs = [\"codingame isola\", \"I'M THE AI\", \"opponentLastRemovedTileX\", \"calculatePossibleRemovedTiles\", \"isola minimax\", \"9x9 isola bot\", \"isola alpha-beta\", \"isola bot programming\"];\nconst arxivQs = [\"all%3Aisolation%20AND%20all%3Aminimax\", \"all%3A%22Isola%22%20AND%20all%3A%22artificial%20intelligence%22\", \"all%3A%22board%20game%22%20AND%20all%3A%22isolation%22\"];\nconst bingQs = [\"isola+codingame\", \"isola+codingame+forum\", \"%22isola%22+%22codingame%22+algorithm\", \"isola+game+bot+programming+heuristic\"];\nconst reqs = [\n  ...grepQs.map(q=>[\"grep:\"+q, \"https://grep.app/api/search?q=\"+encodeURIComponent(q)]),\n  ...arxivQs.map(q=>[\"arxiv:\"+q, \"https://export.arxiv.org/api/query?search_query=\"+q+\"&start=0&max_results=10\"]),\n  ...bingQs.map(q=>[\"bing:\"+q, \"https://www.bing.com/search?q=\"+q+\"&format=rss\"]),\n  [\"brave\", \"https://search.brave.com/search?q=codingame+isola+bot\"],\n  [\"cgpage\", \"https://www.codingame.com/multiplayer/bot-programming/isola\"]\n];\nconst out = await Promise.allSettled(reqs.map(async ([label,u])=>{\n  const r = await fetch(u,{headers:{\"User-Agent\":\"Mozilla/5.0\"}});\n  const t = await r.text();\n  return {label,status:r.status,len:t.length,body:t.slice(0,6000)};\n}));\nreturn out.map((x,i)=>x.status===\"fulfilled\"?{label:reqs[i][0],status:x.value.status,len:x.value.len,body:x.value.body}:{label:reqs[i][0],err:String(x.reason)});"} 
> build · hy4-flash-free
Let me dig deeper into the Isolation AI implementations and try CodinGame forum endpoints.
⚙ execute {"code":"const urls = {\n isolaCpp:\"https://raw.githubusercontent.com/on2valhalla/Isola/master/isola.cpp\",\n justinReadme:\"https://api.github.com/repos/justin-tsai/Isolation/contents/\",\n cgForum:\"https://www.codingame.com/forum/\",\n google1:\"https://www.google.com/search?q=site%3Acodingame.com%2Fforum+isola&gbv=1\",\n yandex:\"https://yandex.com/search/?text=codingame%20isola%20bot\",\n searchcode:\"https://searchcode.com/api/codesearch_I/?q=isola+codingame\",\n gh1:\"https://api.github.com/search/repositories?q=isolation+minimax+in%3Aname%2Cdescription%2Creadme&per_page=20\",\n gh2:\"https://api.github.com/search/repositories?q=isola+game+in%3Aname%2Cdescription%2Creadme&per_page=30\"\n};\nconst out = await Promise.allSettled(Object.entries(urls).map(async ([k,u])=>{\n  const r = await fetch(u,{headers:{\"User-Agent\":\"Mozilla/5.0\"}});\n  const t = await r.text();\n  return {k,status:r.status,len:t.length,body:t};\n}));\nconst res = {};\nfor (let i=0;i<out.length;i++){\n  const o=out[i];\n  if(o.status!==\"fulfilled\"){res[Object.keys(urls)[i]]={err:String(o.reason)};continue;}\n  let body=o.value.body;\n  if(o.value.k===\"isolaCpp\"){\n    const lines=body.split(\"\\n\");\n    body=lines.map((l,j)=>(/heur|eval|component|myMoves|opMoves|filled|MOBIL|score|weight|negamax|negascout|TT|transpos/i.test(l)?(j+1)+\": \"+l:\"\")).filter(Boolean).join(\"\\n\").slice(0,12000);\n  } else if([\"google1\",\"yandex\",\"cgForum\",\"searchcode\"].includes(o.value.k)){\n    body=body.replace(/<script[\\s\\S]*?<\\/script>/gi,\"\").replace(/<style[\\s\\S]*?<\\/style>/gi,\"\").slice(0,5000);\n  } else if(o.value.k.startsWith(\"gh\")){\n    try{const j=JSON.parse(body); body=JSON.stringify({total_count:j.total_count,items:(j.items||[]).map(x=>({full_name:x.full_name,description:x.description,stars:x.stargazers_count}))},null,1);}catch(e){}\n    body=body.slice(0,6000);\n  }\n  res[o.value.k]={status:o.value.status,len:o.value.len,body:body.slice(0,8000)};\n}\nreturn res;"} 
> build · hy4-flash-free
⚙ execute {"code":"const urls = {\n isolaEval:\"https://raw.githubusercontent.com/on2valhalla/Isola/master/isola.cpp\",\n udacityTree:\"https://api.github.com/repos/udacity/AIND-Isolation/git/trees/master?recursive=1\",\n wiki:\"https://fr.wikipedia.org/wiki/Isola_(jeu)\",\n justin:\"https://raw.githubusercontent.com/justin-tsai/Isolation/master/README.md\",\n arxivPaper:\"https://arxiv.org/abs/2505.04525\"\n};\nconst out = await Promise.allSettled(Object.entries(urls).map(async ([k,u])=>{const r=await fetch(u,{headers:{\"User-Agent\":\"Mozilla/5.0\"}});return {k,status:r.status,body:await r.text()};}));\nconst res={};\nfor(let i=0;i<out.length;i++){\n const o=out[i]; const k=Object.keys(urls)[i];\n if(o.status!==\"fulfilled\"){res[k]={err:String(o.reason)};continue;}\n let b=o.value.body;\n if(k===\"isolaEval\"){const L=b.split(\"\\n\");b=L.slice(475,525).map((l,j)=>(j+476)+\": \"+l).join(\"\\n\")+\"\\n---MOVE ORDER---\\n\"+L.slice(925,985).map((l,j)=>(j+926)+\": \"+l).join(\"\\n\");}\n if(k===\"udacityTree\"){try{const j=JSON.parse(b);b=JSON.stringify((j.tree||[]).map(x=>x.path).filter(p=>/player|README|isolation/i.test(p)).slice(0,60),null,1);}catch(e){}}\n if(k===\"wiki\"){b=b.replace(/<script[\\s\\S]*?<\\/script>/gi,\"\").replace(/<style[\\s\\S]*?<\\/style>/gi,\"\").replace(/<[^>]+>/g,\" \").replace(/\\s+/g,\" \").slice(0,9000);}\n if(k===\"arxivPaper\"){b=b.replace(/<script[\\s\\S]*?<\\/script>/gi,\"\").replace(/<[^>]+>/g,\" \").replace(/\\s+/g,\" \").slice(0,5000);}\n res[k]={status:o.value.status,body:b.slice(0,11000)};\n}\nreturn res;"} 
> build · hy4-flash-free
Let me try a few more search routes for CodinGame-specific discussions and academic papers.
⚙ execute {"code":"const urls = {\n ddgLite:\"https://lite.duckduckgo.com/lite/?q=codingame+isola+bot+strategy\",\n ddgHtml:\"https://html.duckduckgo.com/html/?q=%22isola%22+codingame+forum\",\n yahoo:\"https://search.yahoo.com/search?p=codingame+isola+bot\",\n ecosia:\"https://www.ecosia.org/search?q=codingame%20isola%20bot\",\n s2:\"https://api.semanticscholar.org/graph/v1/paper/search/bulk?query=isolation%20game%20heuristic%20minimax&fields=title,year,url&limit=10\",\n udacityReadme:\"https://raw.githubusercontent.com/udacity/AIND-Isolation/master/README.md\",\n udacityPlayer:\"https://raw.githubusercontent.com/udacity/AIND-Isolation/master/sample_players.py\"\n};\nconst out = await Promise.allSettled(Object.entries(urls).map(async ([k,u])=>{const r=await fetch(u,{headers:{\"User-Agent\":\"Mozilla/5.0\"}});return {k,status:r.status,body:await r.text()};}));\nconst res={};\nfor(let i=0;i<out.length;i++){\n const o=out[i]; const k=Object.keys(urls)[i];\n if(o.status!==\"fulfilled\"){res[k]={err:String(o.reason)};continue;}\n let b=o.value.body;\n if([\"ddgLite\",\"ddgHtml\",\"yahoo\",\"ecosia\"].includes(k)){\n   b=b.replace(/<script[\\s\\S]*?<\\/script>/gi,\" \").replace(/<style[\\s\\S]*?<\\/style>/gi,\" \").replace(/<[^>]+>/g,\" \").replace(/&nbsp;/g,\" \").replace(/\\s+/g,\" \").slice(0,5000);\n }\n res[k]={status:o.value.status,body:b.slice(0,7000)};\n}\nreturn res;"} 
> build · hy4-flash-free
Found the real forum domain (`forum.codingame.com`). Let me query its API and search it via DDG Lite.
⚙ execute {"code":"const urls = {\n forumSearch:\"https://forum.codingame.com/search.json?q=isola\",\n forumLatest:\"https://forum.codingame.com/latest.json\",\n ddg1:\"https://lite.duckduckgo.com/lite/?q=site%3Aforum.codingame.com+isola\",\n ddg2:\"https://lite.duckduckgo.com/lite/?q=codingame+isola+alpha+beta+strategy\",\n ddg3:\"https://lite.duckduckgo.com/lite/?q=%22isola%22+codingame+legend+boss\"\n};\nconst out = await Promise.allSettled(Object.entries(urls).map(async ([k,u])=>{const r=await fetch(u,{headers:{\"User-Agent\":\"Mozilla/5.0\"}});return {k,status:r.status,body:await r.text()};}));\nconst res={};\nfor(let i=0;i<out.length;i++){\n const o=out[i];const k=Object.keys(urls)[i];\n if(o.status!==\"fulfilled\"){res[k]={err:String(o.reason)};continue;}\n let b=o.value.body;\n if(k.startsWith(\"ddg\")) b=b.replace(/<script[\\s\\S]*?<\\/script>/gi,\" \").replace(/<style[\\s\\S]*?<\\/style>/gi,\" \").replace(/<[^>]+>/g,\" \").replace(/&nbsp;/g,\" \").replace(/\\s+/g,\" \").slice(0,6000);\n res[k]={status:o.value.status,body:b.slice(0,9000)};\n}\nreturn res;"} 
> build · hy4-flash-free
The forum has only one Isola topic (no strategies). Let me read it and check the referee's boss/config details.
⚙ execute {"code":"const urls = {\n topic:\"https://forum.codingame.com/t/197330.json\",\n topic2:\"https://forum.codingame.com/t/community-puzzle-isola/197330.json\",\n configJs:\"https://raw.githubusercontent.com/wala-fr/CodingameIsola/master/CodingameIsolation/src/main/resources/view/config.js\",\n welcome:\"https://raw.githubusercontent.com/wala-fr/CodingameIsola/master/CodingameIsolation/config/level2/welcome_en.html\",\n boardTest:\"https://raw.githubusercontent.com/wala-fr/CodingameIsola/master/CodingameIsolation/src/test/java/com/codingame/model/BoardTest.java\",\n commits:\"https://api.github.com/repos/wala-fr/CodingameIsola/commits?per_page=30\"\n};\nconst out=await Promise.allSettled(Object.entries(urls).map(async ([k,u])=>{const r=await fetch(u,{headers:{\"User-Agent\":\"Mozilla/5.0\"}});return {k,status:r.status,body:await r.text()};}));\nconst res={};\nfor(let i=0;i<out.length;i++){\n const o=out[i];const k=Object.keys(urls)[i];\n if(o.status!==\"fulfilled\"){res[k]={err:String(o.reason)};continue;}\n let b=o.value.body;\n if(k===\"commits\"){try{const j=JSON.parse(b);b=JSON.stringify(j.map(x=>({sha:x.sha.slice(0,8),msg:x.commit.message.slice(0,120),date:x.commit.author.date})),null,1);}catch(e){}}\n res[k]={status:o.value.status,body:b.slice(0,12000)};\n}\nreturn res;"} 
> build · hy4-flash-free
Let me try extracting the boss win-percentage spreadsheet in-memory and do targeted academic searches.
⚙ execute {"code":"// Try in-memory XLSX parse (no files written)\nconst url=\"https://raw.githubusercontent.com/wala-fr/CodingameIsola/master/CodingameIsolation/src/main/resources/view/boss_percentages/ISOLA%20WIN%20PERCENTAGES.xlsx\";\nlet xlsxResult=\"(skipped)\";\ntry{\n const r=await fetch(url); const buf=new Uint8Array(await r.arrayBuffer());\n const dv=new DataView(buf.buffer);\n const entries={};\n let off=0;\n while(off+30<buf.length && dv.getUint32(off,true)===0x04034b50){\n   const method=dv.getUint16(off+8,true);\n   const csize=dv.getUint32(off+18,true);\n   const nlen=dv.getUint16(off+26,true), elen=dv.getUint16(off+28,true);\n   const name=new TextDecoder().decode(buf.slice(off+30,off+30+nlen));\n   const dstart=off+30+nlen+elen;\n   entries[name]={method,data:buf.slice(dstart,dstart+csize)};\n   off=dstart+csize;\n }\n xlsxResult=\"entries: \"+Object.keys(entries).join(\", \");\n const targets=[\"xl/sharedStrings.xml\",\"xl/worksheets/sheet1.xml\",\"xl/workbook.xml\"];\n for(const t of targets){\n  if(!entries[t]) continue;\n  const e=entries[t];\n  let bytes=e.data;\n  if(e.method===8 && typeof DecompressionStream!==\"undefined\"){\n    const ds=new DecompressionStream(\"deflate-raw\");\n    const stream=new Blob([bytes]).stream().pipeThrough(ds);\n    bytes=new Uint8Array(await new Response(stream).arrayBuffer());\n  }\n  const txt=new TextDecoder().decode(bytes);\n  if(t.includes(\"sharedStrings\")){\n    const strings=[...txt.matchAll(/<si>([\\s\\S]*?)<\\/si>/g)].map(m=>[...m[1].matchAll(/<t[^>]*>([\\s\\S]*?)<\\/t>/g)].map(x=>x[1]).join(\"\"));\n    xlsxResult+=\"\\nSHARED STRINGS:\\n\"+strings.join(\" | \").slice(0,3000);\n  } else if(t.includes(\"sheet1\")){\n    const cells=[...txt.matchAll(/<c r=\"([A-Z]+\\d+)\"([^>]*)>([\\s\\S]*?)<\\/c>|<c r=\"([A-Z]+\\d+)\"([^>]*)\\/>/g)].map(m=>{\n      const ref=m[1]||m[4]; const body=m[3]||\"\"; const v=(body.match(/<v>([\\s\\S]*?)<\\/v>/)||[])[1]||\"\"; return ref+\"=\"+v;\n    });\n    xlsxResult+=\"\\nSHEET1 CELLS:\\n\"+cells.join(\" \").slice(0,6000);\n  } else xlsxResult+=\"\\nWORKBOOK:\\n\"+txt.slice(0,1000);\n }\n}catch(e){xlsxResult=\"XLSX ERROR: \"+String(e);}\n\nconst urls={\n arx1:\"https://export.arxiv.org/api/query?search_query=abs%3A%22isolation%22+AND+abs%3A%22adversarial+search%22&start=0&max_results=10\",\n arx2:\"https://export.arxiv.org/api/query?search_query=ti%3A%22isolation%22+AND+abs%3A%22game%22&start=0&max_results=10\",\n ddgPaper:\"https://lite.duckduckgo.com/lite/?q=arxiv+%22isolation%22+board+game+AI+minimax+heuristic\",\n s2:\"https://api.semanticscholar.org/graph/v1/paper/search?query=isolation%20game%20adversarial%20search%20heuristic&fields=title,year,url,abstract&limit=10\"\n};\nconst out=await Promise.allSettled(Object.entries(urls).map(async ([k,u])=>{const r=await fetch(u,{headers:{\"User-Agent\":\"Mozilla/5.0\"}});return {k,status:r.status,body:await r.text()};}));\nconst res={xlsx:xlsxResult};\nfor(let i=0;i<out.length;i++){\n const o=out[i];const k=Object.keys(urls)[i];\n if(o.status!==\"fulfilled\"){res[k]={err:String(o.reason)};continue;}\n let b=o.value.body;\n if(k===\"ddgPaper\") b=b.replace(/<[^>]+>/g,\" \").replace(/\\s+/g,\" \").slice(0,5000);\n if(k.startsWith(\"arx\")){const titles=[...b.matchAll(/<title>([\\s\\S]*?)<\\/title>/g)].map(m=>m[1]).slice(1);b=\"titles: \"+titles.join(\" || \");}\n res[k]={status:o.value.status,body:b.slice(0,6000)};\n}\nreturn res;"} 
> build · hy4-flash-free
Let me examine the heuristic-analysis articles and check a couple of "isola" repos for CodinGame-specific bots.
⚙ execute {"code":"const urls = {\n heuristicArticle:\"https://rstudio-pubs-static.s3.amazonaws.com/594608_66ae5424cf0b4b8aacd307fc05d1c9b3.html\",\n primaryObjects:\"https://www.primaryobjects.com/2020/01/26/intelligent-heuristics-for-the-game-isolation-using-ai-and-minimax/\",\n hamzaTree:\"https://api.github.com/repos/hamzahanafi11/isola/git/trees/master?recursive=1\",\n topicIsola:\"https://api.github.com/search/repositories?q=topic%3Aisola&per_page=50\",\n gerardoTrees:\"https://api.github.com/repos/GerardoDRM/Isolation-AI/git/trees/master?recursive=1\"\n};\nconst out=await Promise.allSettled(Object.entries(urls).map(async ([k,u])=>{const r=await fetch(u,{headers:{\"User-Agent\":\"Mozilla/5.0\"}});return {k,status:r.status,body:await r.text()};}));\nconst res={};\nfor(let i=0;i<out.length;i++){\n const o=out[i];const k=Object.keys(urls)[i];\n if(o.status!==\"fulfilled\"){res[k]={err:String(o.reason)};continue;}\n let b=o.value.body;\n if([\"heuristicArticle\",\"primaryObjects\"].includes(k)){\n   b=b.replace(/<script[\\s\\S]*?<\\/script>/gi,\" \").replace(/<style[\\s\\S]*?<\\/style>/gi,\" \").replace(/<[^>]+>/g,\" \").replace(/&nbsp;/g,\" \").replace(/&#39;/g,\"'\").replace(/\\s+/g,\" \").slice(0,9000);\n }\n if(k.startsWith(\"gerardo\")||k.startsWith(\"hamza\")){try{const j=JSON.parse(b);b=JSON.stringify((j.tree||[]).map(x=>x.path).slice(0,80));}catch(e){}}\n if(k===\"topicIsola\"){try{const j=JSON.parse(b);b=JSON.stringify((j.items||[]).map(x=>({name:x.full_name,desc:x.description,stars:x.stargazers_count})));}catch(e){}}\n res[k]={status:o.value.status,body:b.slice(0,11000)};\n}\nreturn res;"} 
> build · hy4-flash-free
Let me do a final sweep for any CodinGame Isola bot code in public repos and check the boss history.
⚙ execute {"code":"const repos=[\"GACanova/CodinGame-Bot-Programming\",\"koduma/CodinGame\",\"hardzsi/CodinGame\",\"Tbrady9/CodinGame\",\"SlicedPotatoes/CodinGame\",\"JakubDotPy/codingame_bot_programming\"];\nconst out=await Promise.allSettled(repos.map(async full=>{\n const meta=await (await fetch(\"https://api.github.com/repos/\"+full,{headers:{\"User-Agent\":\"Mozilla/5.0\"}})).json();\n const branch=meta.default_branch||\"master\";\n const tree=await (await fetch(\"https://api.github.com/repos/\"+full+\"/git/trees/\"+branch+\"?recursive=1\",{headers:{\"User-Agent\":\"Mozilla/5.0\"}})).json();\n const hits=(tree.tree||[]).map(x=>x.path).filter(p=>/isol/i.test(p));\n return {full,branch,total:tree.tree?.length,hits:hits.slice(0,20)};\n}));\nconst bossCommit=await (await fetch(\"https://api.github.com/repos/wala-fr/CodingameIsola/commits/0ce05768\",{headers:{\"User-Agent\":\"Mozilla/5.0\"}})).json();\nconst bossFiles=(bossCommit.files||[]).map(f=>({file:f.filename,patch:(f.patch||\"\").slice(0,700)}));\nreturn {repos:out.map((x,i)=>x.status===\"fulfilled\"?x.value:{repo:repos[i],err:String(x.reason)}),bossCommit:{msg:bossCommit.commit?.message,files:bossFiles}};"} 
> build · hy4-flash-free
✗ WebFetch https://raw.githubusercontent.com/wala-fr/CodingameIsola/master/CodingameIsolation/src/main/resources/view/boss_percentages/ISOLA%20WIN%20PERCENTAGES.PNG failed
Error: Unsupported fetched image content type: image/png
✗ WebFetch https://api.semanticscholar.org/graph/v1/paper/search?query=isolation%20game%20minimax&limit=5&fields=title,year,url failed
Error: StatusCode: non 2xx status code (429 GET https://api.semanticscholar.org/graph/v1/paper/search?query=isolation%20game%20minimax&limit=5&fields=title,year,url)
> build · hy4-flash-free
# Deep search — CodinGame Multiplayer **Isola**

**Conclusion liminaire** : le referee officiel est public et lisible. En revanche, **aucun bot public CodinGame-Isola ni post-mortem de meilleur joueur n'a été trouvé** : le forum officiel ne contient qu'un sujet `[Community Puzzle] ISOLA` avec **0 réponse**, et la recherche de code GitHub (via grep.app + API) ne renvoie que le referee. Les stratégies ci-dessous viennent donc du **jeu Isola/Isolation en général**, pas de joueurs CodinGame identifiés — je le signale explicitement.

---

## 1) Faits du referee (sources lues)

Le lien est **officiel** : l'énoncé intégré dit « *The source code of this game is available on this GitHub repo* » et pointe vers `github.com/wala-fr/CodingameIsola`.

### Constantes (URL exacte)
`https://raw.githubusercontent.com/wala-fr/CodingameIsola/master/CodingameIsolation/src/main/java/com/codingame/parameter/Constant.java`

| Constante | Valeur |
|---|---|
| `HEIGHT` / `WIDTH` | 9 / 9 |
| `TEAM_NB` | 2 |
| `FIRST_ROUND_TIME_OUT` | 1000 ms |
| `TIME_OUT` | 100 ms |
| `gameManager.setMaxTurns(HEIGHT*WIDTH)` | 81 (Referee.java) |

Départ : `team 0 = (0, 4)`, `team 1 = (8, 4)` — `(HEIGHT-1)/2 = 4` (Board.java).

### Ordre de résolution d'un tour (Referee.java)
`https://raw.githubusercontent.com/wala-fr/CodingameIsola/master/CodingameIsolation/src/main/java/com/codingame/game/Referee.java`

1. `sendInputs(turn)` : position de soi (uniquement si `turn <= 2`), puis position adverse, puis **dernière tuile retirée par l'adversaire** (`-1 -1` au 1er tour, Board.java).
2. `currentPlayer.execute()` ; lecture d'une ligne ; `split(";")` (message optionnel) ; `trim().toUpperCase()`.
3. Si la sortie est `RANDOM` → `board.calculateRandomMove(teamId, gameManager.getRandom())` ; sinon **exactement 4 entiers** `mx my tx ty`, sinon `InvalidMoveException`.
4. **`board.movePawn(teamId, move)` puis `board.removeTile(teamId, tile)`** — cet ordre est crucial.
5. `removeTile` est aléatoire seulement si `RANDOM` ; sinon c'est la tuile fournie.
6. **Condition de fin** : `if (!board.canMove(1 - teamId)) { currentPlayer.setScore(1); endGame(); }` — c'est l'**adversaire** qui est testé juste après *votre* coup+retrait.
7. Sinon bascule vers `1 - teamId`.

### Validations (Board.java)
`https://raw.githubusercontent.com/wala-fr/CodingameIsola/master/CodingameIsolation/src/main/java/com/codingame/model/Board.java`

- Déplacement : dans `[0,8]²`, pas sur l'adversaire, pas « rester », `|dx|<=1` **et** `|dy|<=1` (donc 8 voisins), case non retirée.
- Retrait : dans le plateau, **pas occupée par un pion**, pas déjà retirée. Comme le retrait est vérifié *après* le déplacement, **on peut retirer la case qu'on vient de quitter** — confirmé par `BoardTest.testRemoveTile`.
- `calculatePossibleMoves()` / `calculatePossibleRemovedTiles()` : listes uniformes utilisées par `RANDOM`.

### Départages / fins anormales (Referee.java, `onEnd`)
- Timeout ou action invalide → `score = -1`, `deactivate`, fin immédiate → l'autre gagne.
- `winId = scores[0] > scores[1] ? 0 : 1` : **en cas d'égalité (scores 0/0, par ex. limite de tours atteinte), c'est le joueur 1 (le second) qui est déclaré gagnant**.
- Avec 81 tuiles et 2 pions, au plus 79 retraits sont possibles ; la fin par « adversaire bloqué » devrait donc toujours survenir avant la limite de 81 tours — déduction arithmétique à partir du code, pas un test observé. **[NON VÉRIFIÉ]**

### Protocole
`https://raw.githubusercontent.com/wala-fr/CodingameIsola/master/CodingameIsolation/config/statement_en.html`
`https://raw.githubusercontent.com/wala-fr/CodingameIsola/master/CodingameIsolation/config/stub.txt`
- Output : `x y tx ty` ou `RANDOM`, optionnellement `;MESSAGE`.
- Quirk lu dans le code : `if (turn <= 2)` renvoie la position propre **aussi au 3e tour global** ; selon que le moteur compte les tours à partir de 0 ou 1, cela peut duplicated l'init pour P0. La numérotation exacte du moteur CodinGame n'a pas été lue. **[NON VÉRIFIÉ]**

### Boss fourni
`https://raw.githubusercontent.com/wala-fr/CodingameIsola/master/CodingameIsolation/config/Boss.java`
```java
System.out.println("RANDOM;I'M THE AI");
```
Le boss inclus dans le paquet officiel joue donc **uniformément au hasard**. L'historique Git contient un commit `add boss win percentages and easy boss` (2022-08-23). L'identité/force des boss des ligues supérieures n'est **pas** dans ce dépôt. **[NON VÉRIFIÉ]**

---

## 2) Stratégies des « meilleurs joueurs » — ce qui est réellement trouvable

### 2.0 Absence de sources CodinGame-specific (vérifié)
- Forum officiel (API Discourse) : `https://forum.codingame.com/search.json?q=isola` → **1 seul topic**, `https://forum.codingame.com/t/community-puzzle-isola/197330.json` → **1 post, 0 réponse**, créé par CommunityBot le 2022-08-25, créateur `@wala`. Aucun post-mortem stratégique.
- GitHub : `api.github.com/search/repositories?q=isola+codingame+bot` → 0 résultat ; `topic:isola` → 3 dépôts (referee + 2 implémentations du jeu, aucun bot CodinGame). grep.app : 0 résultat pour `opponentLastRemovedTileX`, `calculatePossibleRemovedTiles`, `codingame isola`. La recherche de code GitHub authentifiée (401) n'a pas pu être utilisée → absence *absolue* de bots publics reste **[NON VÉRIFIÉ]**.

### 2.1 Jason Mann — `on2valhalla/Isola` (meilleure source algorithmique lue)
URL : `https://raw.githubusercontent.com/on2valhalla/Isola/master/README.txt` et `.../isola.cpp`

- Algorithme : **Negamax / Negascout + alpha-beta + iterative deepening + table de transposition** (150 M buckets). D'après le README, cette combinaison a battu MTDf, killer heuristics, history heuristics et aspiration windows **d'au moins 2 ply**.
- Évaluation exacte lue dans `isola.cpp` (l. 482-516) :
  - si `opMoves == 0` → `MAXINT + myMoves` ; si `myMoves == 0` → `MININT - opMoves + 40` ;
  - sinon `(myMoves - opMoves*3 + 60) * board.count()` ;
  - en fin de partie, si les pions sont dans **deux composantes disjointes** : comparaison des tailles de composantes avec `± CLOSEDCOMPONENT * board.count()` (`CLOSEDCOMPONENT = MAXINT/64`).
- Pourquoi ça marche : le poids **3** sur la mobilité adverse traduit que priver l'adversaire vaut plus que s'étendre ; l'évaluation par composantes évite de se faire enfermer. Plateau en `std::bitset`.
- Réserve : jeu original **8×8**, déplacement = « reine » et tuiles brûlées automatiquement ; à adapter au 9×9 et au retrait libre de CodinGame. **[NON VÉRIFIÉ]** pour toute transposition directe.

### 2.2 Kory Becker — « Analysis of Game Playing Heuristics for Isolation » (2020)
URL : `https://rstudio-pubs-static.s3.amazonaws.com/594608_66ae5424cf0b4b8aacd307fc05d1c9b3.html`

Résultats mesurés (200 parties, adversaire minimax profondeur 3) :

| Heuristique | Formule | Victoires |
|---|---|---|
| random | — | 5,2 % |
| baseline | `own - opp` | 50,0 % |
| simple | `own` | 49,8 % |
| defensive | `2*own - opp` | 49,5 % |
| offensive | `own - 2*opp` | 51,0 % |
| defensive→offensive | switch à `ply/(W*H) > 0.5` | 46,0 % |
| **offensive→defensive** | switch à `ply/(W*H) > 0.5` | **52,2 %** (58 % à profondeur 6) |

Pourquoi : agressif tant que l'espace est grand, défensif quand les cases se raréfient. La profondeur reste le facteur dominant.

### 2.3 Autres implémentations lues
- `https://github.com/justin-tsai/Isolation` + README : minimax alpha-beta ; « maximize own moves while minimizing opponent's, prioritizing the former ».
- `https://raw.githubusercontent.com/udacity/AIND-Isolation/master/sample_players.py` : `open_move_score = own`, `improved_score = own - opp`, `center_score = distance au centre²`, `GreedyPlayer` = 1-ply.
- `https://www.primaryobjects.com/2020/01/26/intelligent-heuristics-for-the-game-isolation-using-ai-and-minimax/` : tutoriel minimax + alpha-beta (même auteur).

### 2.4 Articles arXiv / Semantic Scholar
- **Aucun article spécifique à CodinGame Isola trouvé** (recherches arXiv `isolation AND game`, `isolation AND minimax`, `ti:isolation AND abs:game` lues).
- `https://arxiv.org/abs/2505.04525` — *On some improvements to Unbounded Minimax* (Cohen-Solal & Cazenave, 2025) : tables de transposition, backpropagation, heuristique apprise, technique de complétion. Général, pas Isola.
- `https://arxiv.org/abs/2406.08766` — *Injecting Combinatorial Optimization into MCTS: Application to the board game boop* (Richoux, 2024) : MCTS + optimisation combinatoire, +96 % vs baseline MCTS. Transposable comme idée (guider MCTS), pas vérifié sur Isola. **[NON VÉRIFIÉ]**
- `https://arxiv.org/abs/2409.14180` — *Isolation game on graphs* : théorie des graphes, **jeu différent**.
- API Semantic Scholar : **429 Too Many Requests** à chaque appel → **rien de vérifié** depuis cette source. **[NON VÉRIFIÉ]**

---

## 3) Recommandation concrète et testable pour monter au classement

**Hypothèse testable** : un **alpha-beta à profondeur 4-6 avec évaluation « mobilité pondérée + bascule de phase » et couplage mouvement/retrait restreint** bat le boss fourni (aléatoire) de façon quasi systématique, et exploite une erreur clé des bots naïfs : retirer une tuile près de soi au lieu de retirer une case de fuite de l'adversaire.

Prototype (aucune modification de fichier ici, ceci est un plan) :

1. **Simuleur exact** : bitboard 81 bits + 2 positions ; `move` puis `remove` dans cet ordre ; autoriser le retrait de la case quittée ; victoire si `mobilité(adversaire)==0` après le retrait. Valider contre `BoardTest.java`.
2. **Génération de coups** : au lieu de ~8 × 70 ≈ 560 couples `(déplacement, retrait)`, ne considérer que ~15 tuiles candidates : cases adjacentes au pion adverse, cases adjacentes au mien, **points d'articulation locaux** (BFS), plus la case quittée. Revient à ~120 coups → profondeur bien plus grande dans les 100 ms.
3. **Évaluation** (à A/B tester) :
   - `E1 = own - ownCost*opp` avec `ownCost ∈ {1, 2, 3}` ;
   - `E2 = E1` avec bascule offensive→defensive à `ply/81 ≈ 0,5` ;
   - terminaux `+INF / -INF` ; ajout du score de composante de Mann en fin de partie.
   - Ordre de mouvements + table de transposition Zobrist + iterative deepening sous budget ~90 ms.
4. **Protocole de test** :
   - 200 parties vs même adversaire à profondeur fixe, comparer `E1(k=1/2/3)` et `E2` ; référence：`own - opp` 1-ply.
   - 100 parties vs le boss du dépôt (`RANDOM`) : viser **> 95 % de victoires** ; en dessous, le simulateur est probablement faux (vérifier l'ordre move→remove).
   - Mesurer la profondeur atteinte en 90 ms ; si < 4 en Python, passer à C++/Rust ou durcir l'é lagage de tuiles.
5. **Piège à éviter** : ne pas retirer les cases autour de soi « par réflexe Isolation classique » — ici on peut détruire **n'importe quelle case libre**, donc la valeur d'un retrait se juge sur la mobilité **adverse**.

Limite : le boss des ligues supérieures n'étant pas dans le dépôt, son comportement réel reste **[NON VÉRIFIÉ]** ; la recette ci-dessus est d'abord validée contre le boss aléatoire vérifié, puis contre les boss observés en matchs réels.

---

## 4) Liste de toutes les URL lues

**Referee / jeu**
- https://www.codingame.com/multiplayer/bot-programming/isola (page shell Angular, énoncé non rendu côté serveur)
- https://github.com/wala-fr/CodingameIsola
- https://api.github.com/repos/wala-fr/CodingameIsola/git/trees/master?recursive=1
- https://api.github.com/repos/wala-fr/CodingameIsola/commits?per_page=30
- https://raw.githubusercontent.com/wala-fr/CodingameIsola/master/README.md
- https://raw.githubusercontent.com/wala-fr/CodingameIsola/master/CodingameIsolation/config/statement_en.html
- https://raw.githubusercontent.com/wala-fr/CodingameIsola/master/CodingameIsolation/config/Boss.java
- https://raw.githubusercontent.com/wala-fr/CodingameIsola/master/CodingameIsolation/config/config.ini
- https://raw.githubusercontent.com/wala-fr/CodingameIsola/master/CodingameIsolation/config/stub.txt
- https://raw.githubusercontent.com/wala-fr/CodingameIsola/master/CodingameIsolation/config/level2/welcome_en.html
- https://raw.githubusercontent.com/wala-fr/CodingameIsola/master/CodingameIsolation/src/main/java/com/codingame/game/Referee.java
- https://raw.githubusercontent.com/wala-fr/CodingameIsola/master/CodingameIsolation/src/main/java/com/codingame/game/Player.java
- https://raw.githubusercontent.com/wala-fr/CodingameIsola/master/CodingameIsolation/src/main/java/com/codingame/model/Board.java
- https://raw.githubusercontent.com/wala-fr/CodingameIsola/master/CodingameIsolation/src/main/java/com/codingame/model/Point.java
- https://raw.githubusercontent.com/wala-fr/CodingameIsola/master/CodingameIsolation/src/main/java/com/codingame/parameter/Constant.java
- https://raw.githubusercontent.com/wala-fr/CodingameIsola/master/CodingameIsolation/src/test/java/com/codingame/model/BoardTest.java
- https://raw.githubusercontent.com/wala-fr/CodingameIsola/master/CodingameIsolation/src/main/resources/view/config.js
- https://raw.githubusercontent.com/wala-fr/CodingameIsola/master/CodingameIsolation/src/main/resources/view/boss_percentages/ISOLA%20WIN%20PERCENTAGES.PNG (échec : type image non lisible — **[NON VÉRIFIÉ]**)
- https://raw.githubusercontent.com/wala-fr/CodingameIsola/master/CodingameIsolation/src/main/resources/view/boss_percentages/ISOLA%20WIN%20PERCENTAGES.xlsx (parse en mémoire échoué — **[NON VÉRIFIÉ]**)
- https://api.github.com/repos/eulerscheZahl/RefereeCollection/contents/ (pas de dossier Isola)

**Forum / recherche**
- https://forum.codingame.com/search.json?q=isola
- https://forum.codingame.com/t/197330.json
- https://forum.codingame.com/t/community-puzzle-isola/197330.json
- https://lite.duckduckgo.com/lite/?q=codingame+isola+bot+strategy
- https://lite.duckduckgo.com/lite/?q=site%3Aforum.codingame.com+isola
- https://lite.duckduckgo.com/lite/?q=codingame+isola+alpha+beta+strategy
- https://lite.duckduckgo.com/lite/?q=%22isola%22+codingame+legend+boss
- https://lite.duckduckgo.com/lite/?q=arxiv+%22isolation%22+board+game+AI+minimax+heuristic
- https://duckduckgo.com/html/?q=codingame+isola+strategy+bot ; `.../site%3Agithub.com+codingame+isola+bot` ; `.../isola+game+AI+minimax+alpha-beta+arxiv` (les suivantes ont été captchées)
- https://www.bing.com/search?q=...&format=rss (plusieurs requêtes ; résultats non pertinents)
- https://grep.app/api/search?q=... (`opponentLastRemovedTile`, `codingame isola`, `isola minimax`, `isola alpha-beta`, etc. → 0 résultat)
- https://api.github.com/search/repositories?q=isola+codingame ; `...q=codingame+isola+bot` ; `...q=isola+in:name` ; `...q=isolation+minimax+in:name,description,readme` ; `...q=topic:isola`

**Bots / IA Isolation (non CodinGame)**
- https://github.com/on2valhalla/Isola
- https://api.github.com/repos/on2valhalla/Isola/git/trees/master?recursive=1
- https://raw.githubusercontent.com/on2valhalla/Isola/master/README.txt
- https://raw.githubusercontent.com/on2valhalla/Isola/master/isola.cpp
- https://github.com/justin-tsai/Isolation ; https://raw.githubusercontent.com/justin-tsai/Isolation/master/README.md
- https://github.com/udacity/AIND-Isolation ; https://raw.githubusercontent.com/udacity/AIND-Isolation/master/README.md ; https://raw.githubusercontent.com/udacity/AIND-Isolation/master/sample_players.py
- https://api.github.com/repos/GerardoDRM/Isolation-AI/git/trees/master?recursive=1
- https://api.github.com/repos/hamzahanafi11/isola/git/trees/master?recursive=1
- https://rstudio-pubs-static.s3.amazonaws.com/594608_66ae5424cf0b4b8aacd307fc05d1c9b3.html
- https://www.primaryobjects.com/2020/01/26/intelligent-heuristics-for-the-game-isolation-using-ai-and-minimax/
- https://primaryobjects.github.io/isolation/
- https://fr.wikipedia.org/wiki/Isola_(jeu)

**Académique**
- https://export.arxiv.org/api/query?search_query=all:"isolation"+AND+all:"game"&start=0&max_results=10
- https://export.arxiv.org/api/query?search_query=all:isolation+AND+all:minimax...
- https://export.arxiv.org/api/query?search_query=all:"Isola"+AND+all:"artificial intelligence"...
- https://export.arxiv.org/api/query?search_query=all:"board game"+AND+all:"isolation"...
- https://export.arxiv.org/api/query?search_query=ti:"isolation"+AND+abs:"game"...
- https://arxiv.org/abs/2505.04525 ; https://arxiv.org/abs/2406.08766 ; https://arxiv.org/abs/2409.14180
- https://api.semanticscholar.org/graph/v1/paper/search?... (plusieurs requêtes, toutes **429** → **[NON VÉRIFIÉ]**)
