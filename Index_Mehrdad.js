
var fs = require('fs'),
request = require('request');
var path = require('path');

var download = function(obj,Medias,i,callback ){   //uri, filename, callback){
  i++;
  if(i > Medias.length-1 ) return;
  if (Medias[i] == 0) {
    console.log("not Dl pic",i);
    download(obj,Medias,i,callback);
  }
  else{
    console.log("Iam downloading",i);
    uri = obj.medias[i-1].media_url;

BigName = uri.substr(uri.length - 40 ,uri.length );
  var n = BigName.indexOf("/");
  BigName = BigName.substr(n+1,BigName.length);
  var filename = 'Medias/' + BigName;


    request.head(uri, function(err, res, body){
      console.log('content-type:', res.headers['content-type']);
      console.log('content-length:', res.headers['content-length']);
      console.log('filename=',filename);
      request.get(uri).pipe(fs.createWriteStream(filename));
      if(i==Medias.length-1) {
        console.log("done");
        fs.writeFileSync("done.txt", "1");
      }
      download(obj, Medias, i ,function(){});
    });
    return ; 
  }
};

global.appRoot = path.resolve(__dirname);
//console.log(global.appRoot);
var data = fs.readFileSync(global.appRoot +'/myconfig.txt', 'utf8');
//console.log(data);

var change = 0 ;
var obj = JSON.parse(data);
var cont_length = JSON.stringify(Object.keys(obj.medias).length);
//console.log('cont_length = ',cont_length);
var Medias = new Array();
Medias[0]=0;
//if (cont_length==1) return;
//console.log( obj.medias[0].media_url.length - 4)


/// for logo
var uri = obj.preferences.logo_url;
logoname = obj.preferences.logo_url.substr(obj.preferences.logo_url.length - 40 ,obj.preferences.logo_url.length );
console.log("logoname",logoname);
var nn = logoname.indexOf("/");
//console.log("logoname",logoname.substr(nn+1 ,logoname.length));
//var n = logoname.indexOf(".");
//var extention = logoname.substr(n,logoname.length);
logn = 'Medias/' + logoname.substr(nn+1 ,logoname.length);
console.log("heeeeeeelogo",logn);
console.log("logo:"+logn);
if (  fs.existsSync(logn)){
  var crypto = require('crypto');
  var hash ;
  var data = fs.readFileSync(logn); 
  hash = crypto.createHash('md5').update(data).digest('hex');
  if (hash == obj.preferences.logo_md5 ){ //if their hash were equal nothing
      console.log('no change on logo');
  }

  else {//but if they didn't have same hash we should  delete the old one and download new one
    fs.unlinkSync(global.appRoot + '/' + logn);

    request.head(obj.preferences.logo_url, logn, function(err, res, body){
      console.log('content-type:', res.headers['content-type']);
      console.log('content-length:', res.headers['content-length']);
      console.log('TTTT=',logn);
      console.log('TTTT=',obj.preferences.logo_url);
      request.get(obj.preferences.logo_url).pipe(fs.createWriteStream(logn));
    });
  }

}

else{
    request.head(obj.preferences.logo_url, logn,function(err, res, body){
      console.log('content-type:', res.headers['content-type']);
      console.log('content-length:', res.headers['content-length']);
      console.log('TTTT=',logn);
      console.log('TTTT=',obj.preferences.logo_url);
      
      request.get(obj.preferences.logo_url).pipe(fs.createWriteStream(logn));
    });

  
}
///////////////////logo

for (var i = 1; i < ( 1 + parseInt(cont_length)) ; i++) {
  var uri = obj.medias[i-1].media_url;
  BigName = uri.substr(uri.length - 40 ,uri.length );
  var n = BigName.indexOf("/");
  BigName = BigName.substr(n+1,BigName.length);
  var myname = 'Medias/' + BigName;

  var mypath = global.appRoot + '/' + myname;
  var crypto = require('crypto');
  var hash ;
  console.log(myname,"000");
  //console.log(obj.contents[i-1].media_url.substring (obj.contents[i-1].media_url.length - 4));
  if (fs.existsSync(myname) & obj.medias[i-1].media_md5 != null) {
    data = fs.readFileSync(myname);   
    hash = crypto.createHash('md5').update(data).digest('hex');
    if (hash == obj.medias[i-1].media_md5 ){
      console.log('no change on ',myname);
      Medias[i]=0;
      continue;
    }
        
    else{ 
      fs.unlinkSync(mypath);
      fs.writeFileSync("change.txt", "1");
      console.log('delete',myname);  console.log('download new',myname);
      Medias[i]=1;
      continue ;
     }  
  }

  else {
    fs.writeFileSync("change.txt", "1");
    console.log('I am going to DL ',myname);
    Medias[i]=2;
  }
}

//for (var i = 0; i < 1+parseInt(cont_length) ; i++)
  //console.log("Medias",i,"=",Medias[i]);
download(obj, Medias, 0, function(){});

