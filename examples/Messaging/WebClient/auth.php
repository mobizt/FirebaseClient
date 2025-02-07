<?php

error_reporting(E_ERROR | E_PARSE);

function base64url_encode($data){
    return rtrim(strtr(base64_encode($data), '+/', '-_'), '=');
}

function generateToken($email, $privateKey){

    if($email=="" || $privateKey=="")
    return "";
    
    $base64UrlHeader = base64url_encode(
        json_encode(
            array(
                'alg' => 'RS256',
                'typ' => 'JWT'
            )
        )
    );

    $now = time();
    $str= json_encode(
        array(
            "iss" => $email,
            "sub" => $email,
            "aud" => "https://oauth2.googleapis.com/token",
            "iat" => $now,
            "exp" => $now + 3600,
            "scope" =>"https://www.googleapis.com/auth/devstorage.full_control https://www.googleapis.com/auth/datastore https://www.googleapis.com/auth/userinfo.email https://www.googleapis.com/auth/firebase.database https://www.googleapis.com/auth/cloud-platform https://www.googleapis.com/auth/iam"
        )
            );

    $base64UrlPayload =  base64url_encode(str_replace ("\\","",$str));

    openssl_sign(
        $base64UrlHeader.".".$base64UrlPayload,
        $RSA256,
        $privateKey,
        "sha256WithRSAEncryption"
    );

    $keyRSA256 = base64url_encode($RSA256);
    $jwt = $base64UrlHeader . "." . $base64UrlPayload . "." . $keyRSA256;

    return $jwt;
}

function getAccessToken($email,$privateKey)
{
    if($email=="" || $privateKey=="")
    return "";

    $ch = curl_init();
    $post = [
        'grant_type' => 'urn:ietf:params:oauth:grant-type:jwt-bearer',
        'assertion' => generateToken($email,$privateKey)
    ];

curl_setopt($ch, CURLOPT_URL,"https://oauth2.googleapis.com/token");
curl_setopt($ch, CURLOPT_POST, true);
curl_setopt($ch, CURLOPT_POSTFIELDS,$post);

// Receive server response ...
curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
$server_output = curl_exec($ch);
$json = json_decode($server_output, true);
curl_close($ch);
return isset($json['access_token']) ?$json['access_token']:"";
}

$actual_link = (empty($_SERVER['HTTPS']) ? 'http' : 'https') . "://$_SERVER[HTTP_HOST]$_SERVER[REQUEST_URI]";

$parts = parse_url($actual_link);
parse_str($parts['query'], $query);
$email= urldecode($query['email']);
$privateKey= str_replace(" ", "+",urldecode($query['private_key']));
$privateKey= str_replace("BEGIN+PRIVATE+KEY", "BEGIN PRIVATE KEY",$privateKey);
$privateKey= str_replace("END+PRIVATE+KEY", "END PRIVATE KEY",$privateKey);
$privateKey= str_replace("\\n", "\n",$privateKey);
echo getAccessToken($email,$privateKey);

?>