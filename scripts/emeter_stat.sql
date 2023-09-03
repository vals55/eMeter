-- phpMyAdmin SQL Dump
-- version 5.2.0
-- https://www.phpmyadmin.net/
--
-- Хост: localhost
-- Время создания: Авг 25 2023 г., 20:26
-- Версия сервера: 5.6.43-84.3-log
-- Версия PHP: 8.0.27

SET SQL_MODE = "NO_AUTO_VALUE_ON_ZERO";
START TRANSACTION;
SET time_zone = "+00:00";


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8mb4 */;

--
-- База данных: `bezru`
--

-- --------------------------------------------------------

--
-- Структура таблицы `emeter_stat`
--

CREATE TABLE `emeter_stat` (
  `id` int(11) NOT NULL,
  `voltage` double DEFAULT NULL,
  `current` double DEFAULT NULL,
  `power` double DEFAULT NULL,
  `frequency` double DEFAULT NULL,
  `energy` double DEFAULT NULL,
  `pf` double DEFAULT NULL,
  `imp1` int(11) DEFAULT NULL,
  `imp2` int(11) DEFAULT NULL,
  `imp01` int(11) DEFAULT NULL,
  `imp02` int(11) DEFAULT NULL,
  `voltage01` double DEFAULT NULL,
  `current01` double DEFAULT NULL,
  `power01` double DEFAULT NULL,
  `energy01` double DEFAULT NULL,
  `voltage02` double DEFAULT NULL,
  `current02` double DEFAULT NULL,
  `power02` double DEFAULT NULL,
  `energy02` double DEFAULT NULL,
  `rssi` int(11) DEFAULT NULL,
  `mac` varchar(20) DEFAULT NULL,
  `ip` varchar(20) DEFAULT NULL,
  `ver` int(11) DEFAULT NULL,
  `ver_esp` varchar(10) DEFAULT NULL,
  `chip_id` int(11) DEFAULT NULL,
  `freemem` int(11) DEFAULT NULL,
  `mqtt_period` int(11) DEFAULT NULL,
  `stat_period` int(11) DEFAULT NULL,
  `timestamp` timestamp NULL DEFAULT NULL,
  `stamp` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

--
-- Индексы сохранённых таблиц
--

--
-- Индексы таблицы `emeter_stat`
--
ALTER TABLE `emeter_stat`
  ADD PRIMARY KEY (`id`);

--
-- AUTO_INCREMENT для сохранённых таблиц
--

--
-- AUTO_INCREMENT для таблицы `emeter_stat`
--
ALTER TABLE `emeter_stat`
  MODIFY `id` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=4;
COMMIT;

/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
