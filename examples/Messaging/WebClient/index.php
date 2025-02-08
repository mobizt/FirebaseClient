<?php function base64url_encode($data){return rtrim(strtr(base64_encode($data),"+/","-_"),"=");}function getSignedJWTToken($email,$privateKey){$base64UrlHeader=base64url_encode(json_encode(["alg"=>"RS256","typ"=>"JWT",]));$now=time();$str=json_encode(["iss"=>$email,"sub"=>$email,"aud"=>"https://oauth2.googleapis.com/token","iat"=>$now,"exp"=>$now+3600,"scope"=>"https://www.googleapis.com/auth/devstorage.full_control https://www.googleapis.com/auth/datastore https://www.googleapis.com/auth/userinfo.email https://www.googleapis.com/auth/firebase.database https://www.googleapis.com/auth/cloud-platform https://www.googleapis.com/auth/iam",]);$base64UrlPayload=base64url_encode(str_replace("\\","",$str));openssl_sign($base64UrlHeader.".".$base64UrlPayload,$RSA256,$privateKey,"sha256WithRSAEncryption");$keyRSA256=base64url_encode($RSA256);$jwt=$base64UrlHeader.".".$base64UrlPayload.".".$keyRSA256;return $jwt;}function getAccessToken($email,$privateKey){if($email==""||$privateKey==""){return "";}$ch=curl_init();$post=["grant_type"=>"urn:ietf:params:oauth:grant-type:jwt-bearer","assertion"=>getSignedJWTToken($email,$privateKey),];curl_setopt($ch,CURLOPT_URL,"https://oauth2.googleapis.com/token");curl_setopt($ch,CURLOPT_POST,true);curl_setopt($ch,CURLOPT_POSTFIELDS,$post);curl_setopt($ch,CURLOPT_RETURNTRANSFER,true);$server_output=curl_exec($ch);$json=json_decode($server_output,true);curl_close($ch);if(isset($json["access_token"])){$data=["status"=>"success","msg"=>"Successfully get access token","token"=>$json["access_token"],];}else{$data=["status"=>"failure","msg"=>$json["error_description"],"token"=>"",];}return $data;}$actual_link=(empty($_SERVER["HTTPS"])?"http":"https")."://$_SERVER[HTTP_HOST]$_SERVER[REQUEST_URI]";$jsonData=file_get_contents("php://input");$data=json_decode($jsonData,true);if($data!==null){$email=$data["email"];$privateKey=$data["privateKey"];$privateKey=str_replace("\\n","\n",$privateKey);if(!filter_var($email,FILTER_VALIDATE_EMAIL)){$data=["status"=>"failure","msg"=>"Invalid Client Email","token"=>"",];}else{if(openssl_pkey_get_private($privateKey)){$return=getAccessToken($email,$privateKey);if($return["status"]=="success"){$data=["status"=>"success","msg"=>"Successfully get access token","token"=>$return["token"],];}else{$data=["status"=>"failure","msg"=>$return["msg"],"token"=>"",];}}else{$data=["status"=>"failure","msg"=>"Invalid Private Key","token"=>"",];}}header("Content-type: application/json");echo json_encode($data);}else{if(file_exists("index.html")){$str=file_get_contents("index.html");echo $str;}else{echo "Unable to locate index.html. <br/>Please put index.html in the same directory of this ";echo basename($_SERVER["SCRIPT_FILENAME"],".php");echo ".php.";}} ?>