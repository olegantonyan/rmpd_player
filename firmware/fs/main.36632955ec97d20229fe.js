!function(e){function t(r){if(n[r])return n[r].exports;var i=n[r]={i:r,l:!1,exports:{}};return e[r].call(i.exports,i,i.exports,t),i.l=!0,i.exports}var n={};t.m=e,t.c=n,t.d=function(e,n,r){t.o(e,n)||Object.defineProperty(e,n,{configurable:!1,enumerable:!0,get:r})},t.n=function(e){var n=e&&e.__esModule?function(){return e.default}:function(){return e};return t.d(n,"a",n),n},t.o=function(e,t){return Object.prototype.hasOwnProperty.call(e,t)},t.p="",t(t.s=1)}([function(e,t){"use strict";Object.defineProperty(t,"__esModule",{value:!0}),t.h=function(e,t){for(var n=[],r=[],i=arguments.length;2<i--;)n.push(arguments[i]);for(;n.length;){var u=n.pop();if(u&&u.pop)for(i=u.length;i--;)n.push(u[i]);else null!=u&&!0!==u&&!1!==u&&r.push(u)}return"function"==typeof e?e(t||{},r):{nodeName:e,attributes:t||{},children:r,key:t&&t.key}},t.app=function(e,t,n,r){function i(e){return"function"==typeof e?i(e(O,_)):null==e?"":e}function u(){v=!v;var e=i(n);for(r&&!v&&(m=function e(t,n,r,u,s){if(u===r);else if(null==r||r.nodeName!==u.nodeName){var o=function e(t,n){var r="string"==typeof t||"number"==typeof t?document.createTextNode(t):(n=n||"svg"===t.nodeName)?document.createElementNS("http://www.w3.org/2000/svg",t.nodeName):document.createElement(t.nodeName),u=t.attributes;if(u){u.oncreate&&g.push(function(){u.oncreate(r)});for(var s=0;s<t.children.length;s++)r.appendChild(e(t.children[s]=i(t.children[s]),n));for(var c in u)p(r,c,u[c],null,n)}return r}(u,s);t.insertBefore(o,n),null!=r&&h(t,n,r),n=o}else if(null==r.nodeName)n.nodeValue=u;else{!function(e,t,n,r){for(var i in c(t,n))n[i]!==("value"===i||"checked"===i?e[i]:t[i])&&p(e,i,n[i],t[i],r);var u=j?n.oncreate:n.onupdate;u&&g.push(function(){u(e,t)})}(n,r.attributes,u.attributes,s=s||"svg"===u.nodeName);for(var a={},f={},v=[],d=r.children,m=u.children,b=0;b<d.length;b++){v[b]=n.childNodes[b];var O=l(d[b]);null!=O&&(a[O]=[v[b],d[b]])}for(var b=0,_=0;_<m.length;){var O=l(d[b]),y=l(m[_]=i(m[_]));if(f[O])b++;else if(null!=y&&y===l(d[b+1]))null==O&&h(n,v[b],d[b]),b++;else if(null==y||j)null==O&&(e(n,v[b],d[b],m[_],s),_++),b++;else{var w=a[y]||[];O===y?(e(n,w[0],w[1],m[_],s),b++):w[0]?e(n,n.insertBefore(w[0],v[b]),w[1],m[_],s):e(n,v[b],null,m[_],s),f[y]=m[_],_++}}for(;b<d.length;)null==l(d[b])&&h(n,v[b],d[b]),b++;for(var b in a)f[b]||h(n,a[b][0],a[b][1])}return n}(r,m,b,b=e)),j=!1;g.length;)g.pop()()}function s(){v||(v=!0,setTimeout(u))}function c(e,t){var n={};for(var r in e)n[r]=e[r];for(var r in t)n[r]=t[r];return n}function o(e,t,n){var r={};return e.length?(r[e[0]]=1<e.length?o(e.slice(1),t,n[e[0]]):t,c(n,r)):t}function a(e,t){for(var n=0;n<e.length;)t=t[e[n++]];return t}function l(e){return e?e.key:null}function f(e){return e.currentTarget.events[e.type](e)}function p(e,t,n,r,i){if("key"===t);else if("style"!==t)"o"===t[0]&&"n"===t[1]?(t=t.slice(2),e.events?r||(r=e.events[t]):e.events={},e.events[t]=n,n?r||e.addEventListener(t,f):e.removeEventListener(t,f)):t in e&&"list"!==t&&"type"!==t&&"draggable"!==t&&"spellcheck"!==t&&"translate"!==t&&!i?e[t]=null==n?"":n:null!=n&&!1!==n&&e.setAttribute(t,n),null!=n&&!1!==n||e.removeAttribute(t);else if("string"==typeof n)e.style.cssText=n;else for(var u in"string"==typeof r&&(r=e.style.cssText=""),c(r,n)){var s=null==n||null==n[u]?"":n[u];"-"===u[0]?e.style.setProperty(u,s):e.style[u]=s}}function h(e,t,n){function r(){e.removeChild(function e(t,n){var r=n.attributes;if(r){for(var i=0;i<n.children.length;i++)e(t.childNodes[i],n.children[i]);r.ondestroy&&r.ondestroy(t)}return t}(t,n))}var i=n.attributes&&n.attributes.onremove;i?i(t,r):r()}var v,d=[].map,m=r&&r.children[0]||null,b=m&&function e(t){return{nodeName:t.nodeName.toLowerCase(),attributes:{},children:d.call(t.childNodes,function(t){return 3===t.nodeType?t.nodeValue:e(t)})}}(m),g=[],j=!0,O=c(e),_=function e(t,n,r){for(var i in r)"function"==typeof r[i]?function(e,i){r[e]=function(e){var u=i(e);return"function"==typeof u&&(u=u(a(t,O),r)),u&&u!==(n=a(t,O))&&!u.then&&s(O=o(t,c(n,u),O)),u}}(i,r[i]):e(t.concat(i),n[i]=c(n[i]),r[i]=c(r[i]));return r}([],O,c(t));return s(),_}},function(e,t,n){n(2),n(3),n(4),e.exports=n(5)},function(e,t,n){"use strict";function r(e,t,n){return t in e?Object.defineProperty(e,t,{value:n,enumerable:!0,configurable:!0,writable:!0}):e[t]=n,e}Object.defineProperty(t,"__esModule",{value:!0});var i=n(0),u=function(e){var t,n=e.state,u=e.actions;return Object(i.h)("div",{id:"home"},Object(i.h)("div",{class:"header"},Object(i.h)("h1",null)),Object(i.h)("div",{class:"content"},Object(i.h)("div",{class:"player-control"},Object(i.h)("h2",null,n.status.now_playing),Object(i.h)("input",{disabled:!0,type:"range",min:"0",max:"100",value:n.status.percent_pos,class:"slider",id:"percent-position"}),Object(i.h)("img",{src:"skip_previous-24px.svg",class:"centering",width:"96",height:"96",onclick:function(){return u.home.prev()}}),Object(i.h)("img",{src:"skip_next-24px.svg",class:"centering",width:"96",height:"96",onclick:function(){return u.home.next()}})),Object(i.h)("br",null),Object(i.h)("br",null),Object(i.h)("br",null),Object(i.h)("div",{class:"audio-control"},Object(i.h)("div",{class:"slidecontainer"},Object(i.h)("label",{for:"volume-control",class:"centering"},"Volume "),Object(i.h)("img",{src:"volume_mute-24px.svg",class:"centering"}),Object(i.h)("input",(r(t={type:"range",min:"0",max:"100",step:"5",value:n.status.volume,class:"slider",id:"volume-control"},"class","centering"),r(t,"oninput",function(e){return u.status.update_volume(e.target.value)}),t)),Object(i.h)("img",{src:"volume_up-24px.svg",class:"centering"}),Object(i.h)("span",null,n.status.volume,"%")))))},s=function(e){return e.state,e.actions,Object(i.h)("div",{id:"system"},Object(i.h)("div",{class:"header"},Object(i.h)("h1",null,"System")),Object(i.h)("div",{class:"content"},Object(i.h)("p",null,"Advanced wifi info, logs, web console maybe"),Object(i.h)("button",{class:"pure-button pure-button-primary"},"Reboot")))},c=function(e){var t=e.state,n=e.actions;return Object(i.h)("div",{id:"settings"},Object(i.h)("div",{class:"header"},Object(i.h)("h1",null,"Settings")),Object(i.h)("div",{class:"content"},Object(i.h)("form",{class:"pure-form pure-form-aligned"},Object(i.h)("fieldset",null,Object(i.h)("div",{class:"pure-control-group"},Object(i.h)("label",{for:"wifi_ssid"},"WiFi SSID"),Object(i.h)("input",{id:"wifi_ssid",type:"text",placeholder:"",value:t.settings.wifi_ssid,oninput:function(e){return n.settings.update({wifi_ssid:e.target.value})}})),Object(i.h)("div",{class:"pure-control-group"},Object(i.h)("label",{for:"wifi_pass"},"WiFi password"),Object(i.h)("input",{id:"wifi_pass",type:"text",placeholder:"",value:t.settings.wifi_pass,oninput:function(e){return n.settings.update({wifi_pass:e.target.value})}})),Object(i.h)("div",{class:"pure-controls"},Object(i.h)("a",{class:"pure-button pure-button-primary",onclick:function(){return n.settings.save()},disabled:t.settings.saving},t.settings.saving?"Saving...":"Save"))))))},o=function(e){var t=e.state,n=e.actions;return Object(i.h)("div",{id:"container"},function(e,t){switch(e.nav_current_route){case"home":return Object(i.h)(u,{state:e,actions:t});case"system":return Object(i.h)(s,{state:e,actions:t});case"settings":return Object(i.h)(c,{state:e,actions:t});default:return Object(i.h)(u,{state:e,actions:t})}}(t,n))},a=function(e){var t=e.state;return e.actions,Object(i.h)("footer",{class:"footer"},Object(i.h)("p",null,"Now playing: ",t.status.now_playing," (",t.status.percent_pos,"%) Time: ",t.status.time))},l=function(e){var t=e.state,n=e.actions;return Object(i.h)("div",null,Object(i.h)("a",{id:"menu_link",class:"menu-link",onclick:function(){return n.nav_menu_toggle(!t.nav_menu_open)}},Object(i.h)("span",null)),Object(i.h)("div",{id:"menu"},Object(i.h)("div",{class:"pure-menu"},Object(i.h)("ul",{class:"pure-menu-list"},Object(i.h)("li",{class:"home"===t.nav_current_route?"pure-menu-item pure-menu-selected":"pure-menu-item"},Object(i.h)("a",{class:"pure-menu-link",onclick:function(){return n.nav_route_to("home")}},Object(i.h)("img",{src:"play_circle_filled-24px.svg",class:"centering"}),Object(i.h)("span",{class:"centering"}," Home"))),Object(i.h)("li",{class:"settings"===t.nav_current_route?"pure-menu-item pure-menu-selected":"pure-menu-item"},Object(i.h)("a",{class:"pure-menu-link",onclick:function(){return n.nav_route_to("settings")}},Object(i.h)("img",{src:"settings_applications-24px.svg",class:"centering"}),Object(i.h)("span",{class:"centering"}," Settings"))),Object(i.h)("li",{class:"system"===t.nav_current_route?"pure-menu-item pure-menu-selected":"pure-menu-item"},Object(i.h)("a",{class:"pure-menu-link",onclick:function(){return n.nav_route_to("system")}},Object(i.h)("img",{src:"memory-24px.svg",class:"centering"}),Object(i.h)("span",{class:"centering"}," System")))))))},f=[{nav_menu_open:!1,nav_current_route:"home",settings:{wifi_ssid:null,wifi_pass:null,saving:!1},status:{now_playing:"",percent_pos:0,time:"",volume:0}},{nav_menu_toggle:function(e){return function(){return{nav_menu_open:e}}},nav_route_to:function(e){return function(t,n){switch(e){case"home":case"system":break;case"settings":n.settings.fetch()}return{nav_current_route:e,nav_menu_open:!1}}},home:{prev:function(){return function(){console.log("prev")}},next:function(){return function(){console.log("next")}}},settings:{fetch:function(e){function t(){return e.apply(this,arguments)}return t.toString=function(){return e.toString()},t}(function(){return function(e,t){fetch("/api/settings.json").then(function(e){return e.json()}).then(function(e){return t.update(e)})}}),save:function(){return function(e,t){t.saving(!0),fetch("/api/settings.json",{method:"POST",body:JSON.stringify({wifi_ssid:e.wifi_ssid,wifi_pass:e.wifi_pass}),headers:{"Content-Type":"application/json"}}).then(function(e){return e.json()}).catch(function(){return t.saving(!1)}).then(function(e){t.update(e),t.saving(!1)}).catch(function(){return t.saving(!1)})}},update:function(e){return function(){return e}},saving:function(e){return function(){return{saving:e}}}},status:{fetch:function(e){function t(){return e.apply(this,arguments)}return t.toString=function(){return e.toString()},t}(function(){return function(e,t){fetch("/api/status.json").then(function(e){return e.json()}).then(function(e){return t.update(e)})}}),update:function(e){return function(){return e}},update_volume:function(e){return function(){return fetch("/api/volume.json",{method:"POST",body:JSON.stringify({volume:+e}),headers:{"Content-Type":"application/json"}}),{volume:e}}}}},function(e,t){return Object(i.h)("div",{id:"layout",class:e.nav_menu_open?"active":""},Object(i.h)(l,{state:e,actions:t}),Object(i.h)(o,{state:e,actions:t}),Object(i.h)(a,{state:e,actions:t}))},document.getElementById("app")];!function(e){e.status.fetch(),setInterval(function(){e.status.fetch()},1500)}(i.app.apply(void 0,f))},function(){},function(){},function(){}]);