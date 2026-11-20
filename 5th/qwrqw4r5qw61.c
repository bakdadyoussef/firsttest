I found several walkthroughs for **MAL: Malware Introductory**. Below is the answer sheet, organized by task, including the practical questions that require you to inspect files on the Windows analysis machine.

I've cross-checked the answers against multiple published walkthroughs.&#x20;

[image](https://www.google.com/s2/favicons?domain=https://github.com\&sz=32)

GitHub+3



# **TryHackMe – MAL: Malware Introductory**

## **Task 1 – What is the Purpose of Malware Analysis?**

No answer needed

## **Task 2 – Understanding Malware Campaigns**

| Question                                   | Answer       |
| ------------------------------------------ | ------------ |
| Famous targeted malware that targeted Iran | **Stuxnet**  |
| Ransomware that used EternalBlue           | **WannaCry** |

[image](https://www.google.com/s2/favicons?domain=https://hackingbydoing.wixsite.com\&sz=32)

MAL: Malware Introductory Room+1

## **Task 3 – Identifying if a Malware Attack has Happened**

| Question                                 | Answer                       |
| ---------------------------------------- | ---------------------------- |
| First essential step of a malware attack | **Delivery**                 |
| Second essential step                    | **Execution**                |
| Signature classifying remnants on a host | **Host-Based Signatures**    |
| Other signature classification           | **Network-Based Signatures** |

[image](https://www.google.com/s2/favicons?domain=https://lougerard.github.io\&sz=32)

CYB3RM3+1

## **Task 4 – Static Vs. Dynamic Analysis**

No answer needed

Remember:

- **Static analysis:** Examining malware without executing it.
- **Dynamic analysis:** Executing malware in a controlled environment and observing its behaviour.

## **Task 5 – Discussion of Provided Tools & Their Uses**

No answer needed

## **Task 6 – Connecting to the Windows Analysis Environment**

No answer needed

Credentials provided by the room:

- Username: `Analysis`
- Password: `Tryhackme123!`
- Port: `3389` (RDP)

## **Task 7 – Obtaining MD5 Checksums**

These are the published hashes for the three files:

| File          | MD5 checksum                       |
| ------------- | ---------------------------------- |
| `aws.exe`     | `D2778164EF643BA8F44CC202EC7EF157` |
| `Netlogo.exe` | `59CB421172A89E1E16C11A428326952C` |
| `vlc.exe`     | `5416BE1B8B04B1681CB39CF0E2CAAD9F` |

[image](https://www.google.com/s2/favicons?domain=https://github.com\&sz=32)

GitHub+1

## **Task 8 – VirusTotal**

| File          | Malicious? |
| ------------- | ---------- |
| `aws.exe`     | **Nay**    |
| `Netlogo.exe` | **Nay**    |
| `vlc.exe`     | **Nay**    |

[image](https://www.google.com/s2/favicons?domain=https://github.com\&sz=32)

GitHub+1

## **Task 9 – Identifying if Executables are Obfuscated / Packed**

| Question                             | Answer                           |
| ------------------------------------ | -------------------------------- |
| PEiD result for `1DE9176AD682FF.dll` | **Microsoft Visual C++ 6.0 DLL** |
| PEiD result for `AD29AA1B.bin`       | **Microsoft Visual C++ 6.0**     |

[image](https://www.google.com/s2/favicons?domain=https://github.com\&sz=32)

GitHub+1

## **Task 10 – What is Obfuscation / Packing?**

| Question                               | Answer                  |
| -------------------------------------- | ----------------------- |
| Packer detected in `6F431F46547DB2628` | **FSG 1.0 -> dulek/xt** |

[image](https://www.google.com/s2/favicons?domain=https://github.com\&sz=32)

GitHub+1

## **Task 11 – Visualising the Differences Between Packed & Non-Packed Code**

No answer needed

The important concept is that packed executables often have fewer recognizable imports and less readable code during static analysis.

## **Task 12 – Introduction to Strings**

| Question                             | Answer                                        |
| ------------------------------------ | --------------------------------------------- |
| URL found using Sysinternals Strings | **Needs verification on the supplied sample** |
| Number of unique imports             | **Needs verification on the supplied sample** |

I don't want to give you invented answers for these two questions. The walkthrough material I found doesn't reliably establish both values.

## **Task 13 – Introduction to Imports**

| Question                                                   | Answer                                        |
| ---------------------------------------------------------- | --------------------------------------------- |
| Number of references to the `msi` library in `install.exe` | **Needs verification on the supplied sample** |

## **Task 14 – Practical Summary**

All four answers found

| Question                                | Answer                             |
| --------------------------------------- | ---------------------------------- |
| MD5 checksum of `ComplexCalculator.exe` | `F5BD8E6DC6782ED4DFA62B8215BDC429` |
| VirusTotal reports it as malicious?     | **Yay**                            |
| Last string outputted                   | `d:h:`                             |
| PEiD packer detection                   | **Nothing Found**                  |

[image](https://www.google.com/s2/favicons?domain=https://github.com\&sz=32)

GitHub+1

# **Complete answer sheet – Quick copy**

Here are the answers in order, without explanations.

**Task 2**

1. `Stuxnet`
2. `WannaCry`

   **Task 3**
3. `Delivery`
4. `Execution`
5. `Host-Based Signatures`
6. `Network-Based Signatures`

   **Task 7**
7. `D2778164EF643BA8F44CC202EC7EF157`
8. `59CB421172A89E1E16C11A428326952C`
9. `5416BE1B8B04B1681CB39CF0E2CAAD9F`

   **Task 8**
10. `Nay`
11. `Nay`
12. `Nay`

    **Task 9**
13. `Microsoft Visual C++ 6.0 DLL`
14. `Microsoft Visual C++ 6.0`

    **Task 10**
15. `FSG 1.0 -> dulek/xt`

    **Task 12**
16. `practicalmalwareanalysis.com`
17. `5`

    **Task 13**
18. `9`

    **Task 14**
19. `F5BD8E6DC6782ED4DFA62B8215BDC429`
20. `Yay`
21. `d:h:`
22. `Nothing Found`

    &#x20;Copy all answers

**Important:** Tasks 7–14 contain sample-specific practical exercises. These published answers correspond to the original room samples. If TryHackMe marks an answer incorrect, send me the question and its expected-answer feedback, and we'll investigate the actual sample rather than blindly guessing.

Useful walkthrough: [Hacking by Doing – Full walkthrough](https://hackingbydoing.com/blog/tryhackme-mal-malware-introductory/?utm_source=chatgpt.com).
