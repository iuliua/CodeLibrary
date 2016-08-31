#pragma once
#include <openssl/evp.h>

unsigned char key[] = { 0x6a, 0x46, 0x68, 0x36, 0x69, 0x6e, 0x67, 0x33,
    0x39, 0x39, 0x4d, 0x4e, 0x37, 0x68, 0x6b, 0x4e,
    0x5a, 0x31, 0x6f, 0x53, 0x54, 0x4e, 0x69, 0x7a,
    0x61, 0x45, 0x67, 0x72, 0x42, 0x78, 0x47, 0x37 };

unsigned char iv[] = { 0x41, 0x4c, 0x67, 0x43, 0x33, 0x79, 0x73, 0x6a,
    0x6f, 0x74, 0x36, 0x4a, 0x57, 0x33, 0x79, 0x4b };

int encrypt(unsigned char *plaintext, int plaintext_len, unsigned char *key, unsigned char *iv, unsigned char *ciphertext)
{
    EVP_CIPHER_CTX *ctx;

    int len;

    int ciphertext_len;

    /* Create and initialise the context */
    if (!(ctx = EVP_CIPHER_CTX_new()))
        return 0;

    /* Initialise the encryption operation. IMPORTANT - ensure you use a key
    * and IV size appropriate for your cipher
    * In this example we are using 256 bit AES (i.e. a 256 bit key). The
    * IV size for *most* modes is the same as the block size. For AES this
    * is 128 bits */
    if (1 != EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv))
        return 0;

    /* Provide the message to be encrypted, and obtain the encrypted output.
    * EVP_EncryptUpdate can be called multiple times if necessary
    */
    if (1 != EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, plaintext_len))
        return 0;
    ciphertext_len = len;

    /* Finalise the encryption. Further ciphertext bytes may be written at
    * this stage.
    */
    if (1 != EVP_EncryptFinal_ex(ctx, ciphertext + len, &len))
        return 0;
    ciphertext_len += len;

    /* Clean up */
    EVP_CIPHER_CTX_free(ctx);

    return ciphertext_len;
}
int decrypt(unsigned char *ciphertext, int ciphertext_len, unsigned char *key, unsigned char *iv, unsigned char *plaintext)
{
    EVP_CIPHER_CTX *ctx;

    int len;

    int plaintext_len;

    /* Create and initialise the context */
    if (!(ctx = EVP_CIPHER_CTX_new()))
        return 0;

    /* Initialise the decryption operation. IMPORTANT - ensure you use a key
    * and IV size appropriate for your cipher
    * In this example we are using 256 bit AES (i.e. a 256 bit key). The
    * IV size for *most* modes is the same as the block size. For AES this
    * is 128 bits */
    if (1 != EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv))
        return 0;

    /* Provide the message to be decrypted, and obtain the plaintext output.
    * EVP_DecryptUpdate can be called multiple times if necessary
    */
    if (1 != EVP_DecryptUpdate(ctx, plaintext, &len, ciphertext, ciphertext_len))
        return 0;
    plaintext_len = len;

    /* Finalise the decryption. Further plaintext bytes may be written at
    * this stage.
    */
    if (1 != EVP_DecryptFinal_ex(ctx, plaintext + len, &len))
        return 0;
    plaintext_len += len;

    /* Clean up */
    EVP_CIPHER_CTX_free(ctx);

    return plaintext_len;
}
class CLicenseFile
{
public:
    
    static BOOL IsLicenseValid(const std::string arg_plugin_name, const std::string &mt_version, const std::string &mt_company_name)
    {
        char path[MAX_PATH];
        HMODULE hm = NULL;

        if (!GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
            GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
            (LPCSTR)&IsLicenseValid,
            &hm))
            return FALSE;
        GetModuleFileNameA(hm, path, sizeof(path));

        std::string full_path_license_file(path);
        full_path_license_file=full_path_license_file.substr(0, full_path_license_file.size() - 3) + "lic";
        int ret = FALSE;
        HANDLE hFile = CreateFileA(full_path_license_file.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        if (hFile != INVALID_HANDLE_VALUE)
        {
            UCHAR buffer[1024];
            DWORD size = 0;
            if (ReadFile(hFile, buffer, 1024, &size, 0))
            {
                UCHAR output[1024];
                unsigned char random_seed[32];
                decrypt(buffer, 32, key, iv, random_seed);
                int outsize = decrypt(buffer + 32, size - 32, key, random_seed, output);
                output[outsize] = 0;
                std::string license_string;
                license_string.assign((char*)output);
                ret = ValidLicense(license_string,arg_plugin_name,mt_version,mt_company_name);
            }
            CloseHandle(hFile);
        }
        return ret;
    }
    static BOOL ValidLicense(const std::string &license_string, const std::string arg_plugin_name, const std::string &mt_version, const std::string &mt_company_name) 
    {
        //Only for test;2016-06-2
        std::istringstream iss(license_string);
        std::string version;
        std::string plugin_name;
        std::string expiry_date;
        std::string company_name;

        std::getline(iss, version, ';');
        if (version != mt_version)
            return FALSE;
        std::getline(iss, plugin_name, ';');
        std::getline(iss, company_name, ';');
        std::getline(iss, expiry_date, ';');
        if (plugin_name != arg_plugin_name)
            return FALSE;
        if (mt_company_name != company_name)
            return FALSE;

        if (CTrialPeriod::Expired(expiry_date))
            return FALSE;
        return TRUE;
    }
};